#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <newt.h>

// SIGTSTP
#include <signal.h>


struct callbackInfo {
    newtComponent en;
    char * state;
};

void disableCallback(newtComponent co, void * data) {
    struct callbackInfo * cbi = data;

    if (*cbi->state == ' ') {
         newtEntrySetFlags(cbi->en, NEWT_FLAG_DISABLED, NEWT_FLAGS_RESET);
    } else {
         newtEntrySetFlags(cbi->en, NEWT_FLAG_DISABLED, NEWT_FLAGS_SET);
    }

    newtRefresh();
}

void suspend(void * d) {
    newtSuspend();
    raise(SIGTSTP);
    newtResume();
}

void helpCallback(newtComponent co, void * tag) {
    newtWinMessage("Help", "Ok", tag);
}
//=============================================================================
int main(void) {
    newtComponent b1, b2, r1, r2, r3, e2, e3, l1, l2, l3, scale;
    newtComponent lb, t, rsf, answer, timeLabel;
    newtComponent cs[10];
    newtComponent f, chklist, e1;
    struct callbackInfo cbis[3];
    char results[10];//----------------------------返回值
    const char * enr2, * enr3, * scaleVal;
    void ** selectedList;
    int i, numsel;
    char buf[20];//--------------------------------缓冲区
    const char * spinner = "-\\|/\\|/"; //--------------
    const char * spinState;
    struct newtExitStruct es;

    newtInit();//---------------------------------初使化
    newtCls(); //---------------------------------清屏
//================================================================================
    newtSetSuspendCallback(suspend, NULL);
    newtSetHelpCallback(helpCallback);
//================================================================================
    newtDrawRootText(0, 0, "Newt test program"); //在根窗体上输出字符串
    newtPushHelpLine(NULL);
    newtDrawRootText(-50, 0, "More root text");  //在根窗体上输出字符串
                 //----------------------------------------------------------------------------
    newtOpenWindow(2, 2, 30, 10, "first window"); //----新建窗口
    newtOpenWindow(10, 5, 65, 16, "window 2");    //----新建窗口
//==================================================================================
    f = newtForm(NULL, "This is some help text", 0); //===============新建一个FORM 表单
    chklist = newtForm(NULL, NULL, 0);//-----------------------------新建一个FORM 表单


    b1 = newtButton(3, 1, "Exit");     //----按钮
    b2 = newtButton(18, 1, "Update");  //----按钮
    r1 = newtRadiobutton(20, 10, "Choice 1", 0, NULL);//----单选按钮
    r2 = newtRadiobutton(20, 11, "Chc 2", 1, r1);     //----单选按钮
    r3 = newtRadiobutton(20, 12, "Choice 3", 0, r2);  //----单选按钮

    rsf = newtForm(NULL, NULL, 0);//-----------------------------新建一个FORM 表单
    newtFormAddComponents(rsf, r1, r2, r3, NULL);//---------------------------------向表单加入物件
    newtFormSetBackground(rsf, NEWT_COLORSET_CHECKBOX);//------------设置表单背景颜色

    for (i = 0; i < 10; i++) {
        sprintf(buf, "Check %d", i);//--向缓冲区中写入数据
        cs[i] = newtCheckbox(3, 10 + i, buf, ' ', NULL, &results[i]);//--建立检查框---一共10项
        newtFormAddComponent(chklist, cs[i]);//---向表单中加入组件
    }

    l1 = newtLabel(3, 6, "Scale:");   //--------------标签
    l2 = newtLabel(3, 7, "Scrolls:"); //--------------标签
    l3 = newtLabel(3, 8, "Hidden:");  //--------------标签
    e1 = newtEntry(12, 6, "", 20, &scaleVal, 0); //---------------输入框
    e2 = newtEntry(12, 7, "Default", 20, &enr2, NEWT_FLAG_SCROLL);//---输入框
/*    e3 = newtEntry(12, 8, NULL, 20, &enr3, NEWT_FLAG_HIDDEN); */
    e3 = newtEntry(12, 8, NULL, 20, &enr3, NEWT_FLAG_PASSWORD);  //--------输入框
//======================================================================================
    cbis[0].state = &results[0];
    cbis[0].en = e1;
    newtComponentAddCallback(cs[0], disableCallback, &cbis[0]);

    scale = newtScale(3, 14, 32, 100);  //------------------刻度
    newtScaleSet(scale,50);             //------------------------------设置刻度值
    newtFormSetHeight(chklist, 3);      //-----------设置表单高

    newtFormAddComponents(f, b1, b2, l1, l2, l3, e1, e2, e3, chklist, NULL);//向表单中填加组件
    newtFormAddComponents(f, rsf, scale, NULL);//向表单中填加组件

    lb = newtListbox(45, 1, 6, NEWT_FLAG_MULTIPLE | NEWT_FLAG_BORDER |
                                   NEWT_FLAG_SCROLL | NEWT_FLAG_SHOWCURSOR);
    newtListboxAppendEntry(lb, "First", (void *) 1);
    newtListboxAppendEntry(lb, "Second", (void *) 2);
    newtListboxAppendEntry(lb, "Third", (void *) 3);
    newtListboxAppendEntry(lb, "Fourth", (void *) 4);
    newtListboxAppendEntry(lb, "Sixth", (void *) 6);
    newtListboxAppendEntry(lb, "Seventh", (void *) 7);
    newtListboxAppendEntry(lb, "Eighth", (void *) 8);
    newtListboxAppendEntry(lb, "Ninth", (void *) 9);
    newtListboxAppendEntry(lb, "Tenth", (void *) 10);

    newtListboxInsertEntry(lb, "Fifth", (void *) 5, (void *) 4);
    newtListboxInsertEntry(lb, "Eleventh", (void *) 11, (void *) 10);
    newtListboxDeleteEntry(lb, (void *) 11);

    spinState = spinner;
    timeLabel = newtLabel(45, 8, "Spinner: -");

    t = newtTextbox(45, 10, 17, 5, NEWT_FLAG_WRAP);
    newtTextboxSetText(t, "This is some text does it look okay?\nThis should be alone.\nThis shouldn't be printed");

    newtFormAddComponents(f, lb, timeLabel, t, NULL);
    newtRefresh();
    newtFormSetTimer(f, 200);

    do {
         newtFormRun(f, &es);

         if (es.reason == NEWT_EXIT_COMPONENT && es.u.co == b2) {
             newtScaleSet(scale, atoi(scaleVal));
             newtRefresh();
             answer = NULL;
         } else if (es.reason == NEWT_EXIT_TIMER) {
             spinState++;
             if (!*spinState) spinState = spinner;
             sprintf(buf, "Spinner: %c", *spinState);
             newtLabelSetText(timeLabel, buf);
         }
    } while (es.reason != NEWT_EXIT_COMPONENT || es.u.co == b2);

    scaleVal = strdup(scaleVal);
    enr2 = strdup(enr2);
    enr3 = strdup(enr3);

    selectedList = newtListboxGetSelection(lb, &numsel);

    newtFormDestroy(f);

    newtPopWindow();
    newtPopWindow();
    newtFinished();

    printf("got string 1: %s\n", scaleVal);
    printf("got string 2: %s\n", enr2);
    printf("got string 3: %s\n", enr3);

    if(selectedList) {
         printf("\nSelected listbox items:\n");
         for(i = 0; i < numsel; i++)
             puts(selectedList[i]);
    }

    return 0;
}
