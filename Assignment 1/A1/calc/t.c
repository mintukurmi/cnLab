#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int calc(char ch[255]);

int main(){

    calc("2+3");

   return 0;

}

int calc(char ch[255]){

int num1 = 0,num2 = 0;
char op;
int state = 0;
int result =0;

    int i=0;
    for (i = 0; i < strlen(ch); i++) {
            if (ch[i] != '\n'){

                if (ch[i] >= '0' && ch[i]  <= '9'){ //Checks if the character is a number
                    if (state == 0) 
                    num1 = num1*10 + ch[i] - '0'; // Convert ASCII to decimal
                    else 
                    num2 = num2*10 + ch[i] - '0'; // Convert ASCII to decimal
            }
            else{
                op = ch[i] ;
                state = 1;
            }

        }     

    }

       switch(op)
       {
         case '+':
            result = num1 + num2;
         break;
         case '-':
            result = num1 - num2;
         break;
         case '*':
            result = num1 * num2;
         break;
         case '/':
            result = num1 / num2;
         break;
       }
   
    printf("%d %c %d = %d", num1, op, num2, result);
}