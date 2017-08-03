#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

#include <sys/stat.h> 
#include <sys/types.h> 

#include <newt.h>

int main(int argc, char *argv[]) 
         {
                 int i;
                 //定义组件
                 newtComponent form;
                 newtComponent label_checkbox, label_radiobutton;
                 newtComponent checkbox[3], radiobutton[3];
                 newtComponent button_ok;
                 newtComponent co;
                 char result_checkbox[3];
                 int  result_radiobutton;
 
                 char *str_checkbox[3] = {
                          "computer",
                          "mourse",
                          "keyboad"
                 };
                 char *str_radiobutton[3] = {
                          "Linux",
                          "Windows",
                          "Macintosh"
                 };
 
 
                 //初始化
                 newtInit();
                 newtCls();
 
                 //建立窗口
                 newtCenteredWindow(60,15, "Buttons Sample");
 
                 //-------------------------------------------
                 //建立Form
                 form = newtForm(NULL, NULL, 0);
 
                 //多选按钮
                 label_checkbox = newtLabel(10, 1, "hardwork list");
                 newtFormAddComponent(form, label_checkbox);
                 for(i=0; i<3; i++){
                          checkbox[i] = newtCheckbox(10, 3 + i, str_checkbox[i],
                                   '#', " *", NULL);
                          newtFormAddComponent(form, checkbox[i]);
                 }
 
                 label_radiobutton = newtLabel(30, 1, "software list");
                 newtFormAddComponent(form, label_radiobutton);
 
 
                 radiobutton[0] = newtRadiobutton(30, 3, str_radiobutton[0], 
                          1, NULL);
                  radiobutton[1] = newtRadiobutton(30, 4, str_radiobutton[1], 
                          0, radiobutton[0]);
                 radiobutton[2] = newtRadiobutton(30, 5, str_radiobutton[2], 
                          0, radiobutton[1]);
                 for(i=0; i<3; i++)
                          newtFormAddComponent(form, radiobutton[i]);
 
                 button_ok = newtButton(20, 7, "Confirm");
                 newtFormAddComponent(form, button_ok);
 
                 //--------------------------------------------
                 //进入循环
                 newtRunForm(form);
 
                 //获取结果
 
                 for(i=0; i<3; i++)
                          result_checkbox[i] = newtCheckboxGetValue(checkbox[i]);
                 co = newtRadioGetCurrent(radiobutton[0]);
                 for(i=0; i<3; i++){
                          if(co == radiobutton[i]) result_radiobutton = i;
                 }
 
 
                 //关闭Form
                 newtFormDestroy(form);
 
                 //复原
                 newtFinished();
 
                 //打印结果
                 printf("hardwork choice:\n");
                 for(i=0; i<3; i++)
                          if(result_checkbox[i] == '*')
                                   printf("[*] %s\n", str_checkbox[i]);
                 printf("software choice:\n");
                 printf("(*) %s\n", str_radiobutton[result_radiobutton]);
 
         }