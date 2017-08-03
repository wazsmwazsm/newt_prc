#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

#include <sys/stat.h> 
#include <sys/types.h> 

#include <newt.h>

// 处理项目数，为了以后增加项目维护方便
#define MAX_ITEM 15


/*
    还原函数

*/
void revert()   
{
    //容器组件定义
    newtComponent form, entry, button;   
    //接收输入数据
    char * entryValue; 

    //管道消息参数定义
    FILE * fp;
    char buffer[80];


    // open new window and form  
    newtCls();   
    newtRefresh();
    newtPopWindow();   


    // 创建输入框


    newtCenteredWindow(50,10,"Input the IMG file name you want to revert");   
    entry = newtEntry(16, 1, NULL, 20, (const char **)&entryValue, NEWT_FLAG_SCROLL);   
    newtEntrySet(entry,"\0",0);   
    button = newtButton(20, 5, "Confirm");   
    form = newtForm(NULL,NULL, 0);   
    newtFormAddComponents(form, entry, button, NULL);   
    newtRunForm(form);   

    if(*entryValue !='\0') {  // 输入不为空
        //这里销毁容器的话会将entryValue销毁
        //newtFormDestroy(form); // tips: do this will destory entryValue
        newtPopWindow();
       
        if(access(entryValue, 0) == 0){  // img文件存在      
            //容器组件定义
            newtComponent form, back, enter, statu;
            //选择框定义
            newtComponent tar_files;
            //选择状态变量
            char value[MAX_ITEM];
            //循环计数变量
            int i; 
            //read tar files number counter
            int tar_count = 0;
            //脚本文件名
            char scriptsArgus[MAX_ITEM][80];

            //解压命令字符串
            char tar_command[80] = "./sh/dp_img.sh ";
            //创建缓存文件夹
            if(access(".rvcache",0) == -1){
                if(mkdir(".rvcache",0777)){
                    newtWinMessage("ERROR", "Ok", "Create cache Faild, Permission denied.");
                }
            }

            
            // open new window and form           
            //创建窗口
            newtCenteredWindow(50, 20, "Select items you want revert");  
            //创建容器 
            form = newtForm(NULL, NULL, 0); 
            //创建form组件
            enter = newtButton(26, 15, " Confirm"); 
            back = newtButton(10, 15, " << back");
            //将组件添加到容器
            newtFormAddComponent(form, back);
            newtFormAddComponent(form, enter);
            

            // 拼凑命令字符串
            strcat(tar_command, entryValue);

            //读取img文件中的备份信息
            fp = popen(tar_command, "r");
            while(fgets(buffer, sizeof(buffer), fp) != NULL && buffer[0] != '\n'){
                //保存img文件中的备份信息
                strcpy(scriptsArgus[tar_count], buffer);
                tar_files = newtCheckbox(10, tar_count+1, buffer, ' ', " *", &value[tar_count++]);            
                newtFormAddComponent(form, tar_files);
            }
            pclose(fp);

            //添加帮助消息
            newtPushHelpLine(" Press <SPACE> to select, click Confirm button to process");

            //容器运行，返回值为按钮状态
            statu = newtRunForm(form);
            
            //退出
            if(statu == back){
                newtFormDestroy(form);
                newtPopWindow();
                // 删除缓存
                if(system("rm -rf ./.rvcache") < 0){
                    newtWinMessage("Tips", "Ok", "Cache delete failed.");
                }
                return;
            }

            //确认执行
            if(statu == enter){
                //显示位置的计数器
                int count = 1;
                // 销毁上一个窗口和容器
                newtFormDestroy(form);
                newtPopWindow();

                //对选中的项目进行操作
                for (int i = 0; i <= tar_count; ++i){
                    
                    if(value[i] == '*'){                        
                        //命令字符串
                        //给足够空间防止访问越界
                        char command[80] = "./sh/do_rvt.sh  ";

                        //显示进度字符串
                        char pking[80] = "Reverting ";

                        //拼接命令
                        strcat(command, scriptsArgus[i]);
                        strcat(pking, scriptsArgus[i]);
                        //执行脚本
                        fp = popen(command, "r");
                        //从脚本取得done、failed信息到buffer
                        fgets(buffer, sizeof(buffer), fp);
                        newtDrawRootText(5, count*2, pking);
                        newtRefresh();               
                        newtDrawRootText(5, count*2+1, buffer);
                        newtRefresh();
                        
                        pclose(fp);
                        
                        count++;
                    }
                }
                newtWinMessage("Done", "Ok", "All operations have been completed.");

                // 删除缓存
                if(system("rm -rf ./.rvcache") < 0){
                    newtWinMessage("Tips", "Ok", "Cache delete failed.");
                }


                return;
            }       

        } else { // img文件不存在
            newtWinMessage("ERROR", "Ok", "File is not exist.");

        }
 
    } else { //没有输入
        newtPopWindow();
        newtWinMessage("ERROR", "Ok", "No entry !");
    }  
}

/*
    备份函数

*/
void back_up()   
{   
    //容器组件定义
    newtComponent form, back, enter, statu;
    //选择框定义
    newtComponent backup_files;
    //选择状态变量
    char value[MAX_ITEM];
    //循环计数变量
    int i=1; 
    //read tar files number counter
    int list_count = 0;
    //脚本文件名
    char scriptsArgus[MAX_ITEM][80];
    //file source
    FILE * fp;
    char buffer[80];



    // open new window and form  
    newtCls();   
    newtRefresh();
    newtPopWindow();

    //创建窗口
    newtCenteredWindow(50, 20, "Select items you want back up");  
    //创建容器 
    form = newtForm(NULL, NULL, 0); 
    //创建form组件
    enter = newtButton(26, 15, " Confirm"); 
    back = newtButton(10, 15, " << back");
    //将组件添加到容器
    newtFormAddComponent(form, back);
    newtFormAddComponent(form, enter);   

     
    //打开配置文件
    fp = fopen("./conf/packagefiles.cnf", "r");
    
    while(fgets(buffer, sizeof(buffer), fp) != NULL){
        //跳过空行和注释
        if(buffer[0] == '\n' || buffer[0] == '#') continue;

        strcpy(scriptsArgus[list_count], buffer);
        backup_files = newtCheckbox(10, list_count+1, buffer, ' ', " *", &value[list_count++]);            
        newtFormAddComponent(form, backup_files);
    }
    fclose(fp);

    //添加帮助消息
    newtPushHelpLine(" Press <SPACE> to select, click Confirm button to process");

    //容器运行，返回值为按钮状态
    statu = newtRunForm(form);

    //退出
    if(statu == back){
        newtFormDestroy(form);
        newtPopWindow();
        return;
    }

    //确认执行
    if(statu == enter){
        //显示位置的计数器
        int count = 1;
        // 销毁上一个窗口和容器
        //newtFormDestroy(form);
        newtPopWindow();
        
        newtDrawRootText(5, 1, "Backing up ......");
        newtRefresh();


        //创建缓存文件夹
        if(access(".pkcache",0) == -1){
            if(mkdir(".pkcache",0777)){
                newtWinMessage("ERROR", "Ok", "Create cache Faild, Permission denied.");
            }
        } 


        //对选中的项目进行操作
        for (int i = 0; i <= list_count; ++i){
            
            if(value[i] == '*'){                        
                //命令字符串
                //给足够空间防止访问越界
                char command[80] = "./sh/do_pk.sh  ";

                //显示进度字符串
                char pking[80] = "Backing up ";

                //拼接命令
                strcat(command, scriptsArgus[i]);
                strcat(pking, scriptsArgus[i]);
                //执行脚本
                fp = popen(command, "r");
                //从脚本取得done、failed信息到buffer
                fgets(buffer, sizeof(buffer), fp);
                newtDrawRootText(5, count*2, pking);
                newtRefresh();               
                newtDrawRootText(5, count*2+1, buffer);
                newtRefresh();
                
                pclose(fp);
                
                count++;
            }
        }

        //压缩成IMG镜像，删除缓存文件
        newtDrawRootText(5, count*2, "Pcakaging all files......");
        newtRefresh();
        //执行脚本
        if(system("./sh/pk_img.sh") < 0){
            newtWinMessage("Error", "Ok", "Package all file faild.");
        } else {
            newtWinMessage("Done", "Ok", "All operations have been completed.");
        }

    }

}


int main(){

    newtComponent ls,fm;
    int p = 1, q = 2, r = 3, *u;
    newtInit(); 

    do {   
       newtCls();   
       newtRefresh();   
       newtDrawRootText(0,0,"Ubuntu backup & revert program");   
       newtCenteredWindow(50,10,"Please select");   
       ls = newtListbox(18,3,5,NEWT_FLAG_RETURNEXIT);   
       newtListboxAppendEntry(ls,"Back Up", &p);   
       newtListboxAppendEntry(ls,"Revert", &q);     
       newtListboxAppendEntry(ls,"Quit ", &r);   
       newtPushHelpLine(" Move using the arrow keys and press <ENTER> to select");

       // create and run form 
       fm = newtForm(NULL,NULL,0);   
       newtFormAddComponents(fm,ls,NULL);   
       newtRunForm(fm);   
       u = newtListboxGetCurrent(ls);
       newtPopWindow();   
       newtFormDestroy(fm);  

       switch(*u) {   
            case 1:   
                back_up(); 
                break;   
            case 2:   
                revert();
                break;     
            case 3:   
                newtFinished();   
                exit(0);   
        }   
    } while(1);


    return 0;

}
