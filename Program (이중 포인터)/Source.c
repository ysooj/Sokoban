#include <stdio.h>
#include <stdlib.h>

#define SIZE 4

struct GameObject
{
    int x;
    int y;
};

int main()
{
    // 포인터 변수를 저장하는 것.
    // 자료형  변수이름
    // 64비트 컴파일러에서 8 byte 메모리를 차지함
    // 참고로 '&'는 '레퍼런스'라고 읽음.

    // int x = 10;
    // 
    // int * pointer = &x;
    // 
    // int  doublePointer = &pointer;
    // 
    // printf("%p\n", &x);                // x의 주소 값
    // printf("%p\n", pointer);        // x의 주소 값을 저장한 pointer 값
    // printf("%p\n", doublePointer);    // x의 주소 값을 저장한 pointer 값을 doublePointer로 역참조한 값
    // 
    // printf("pointer의 주소 : %p\n", &pointer);                        // pointer의 주소 값
    // printf("doublePointer의 값 : %p\n", doublePointer);                // pointer의 주소 값을 저장한 doublePointer 값
    // 
    // printf("doublePointer가 가리키는 값 : %d\n", **doublePointer);    // doublePointer를 두 번 역참조하여 알게 되는 x 값



    // 구조체를 가리키는 포인터 배열을 동적으로 할당해보자.

    // [수업 시간에 따라했는데, 오류가 많다.]

    //  struct GameObject list[SIZE];
    //  
    //  struct GameObject doublePointer;
    //  
    //  for (int i = 0; i < SIZE; i++)
    //  {
    //      list[i] = malloc(sizeof(struct GameObject));
    //  
    //      doublePointer = &list[i];
    //  
    //      (doublePointer).x = 10;
    //      (**doublePointer).y = 20;
    //  
    //      printf("list[%d].x = %d\n", i, list[i]->x);
    //      printf("list[%d].y = %d\n", i, list[i]->y);
    //  }
    //  
    //  for (int i = 0; i < SIZE; i++)
    //  {
    //      free(list[i]);
    //  }


    // [chatGPT가 수정해준 버전]
    // 구조체 포인터 배열 동적 할당
    struct GameObject* list[SIZE]; // 포인터 배열로 수정
    struct GameObject* doublePointer; // 구조체 포인터로 수정

    for (int i = 0; i < SIZE; i++)
    {
        list[i] = (struct GameObject*)malloc(sizeof(struct GameObject)); // 포인터 배열이므로 malloc 가능

        doublePointer = list[i]; // 구조체 포인터를 포인터에 저장

        doublePointer->x = 10;
        doublePointer->y = 20;

        printf("list[%d]->x = %d\n", i, list[i]->x);
        printf("list[%d]->y = %d\n", i, list[i]->y);
    }

    for (int i = 0; i < SIZE; i++)
    {
        free(list[i]); // 동적 할당 해제
    }


    return 0;
}

// 구조체와 이중 포인터에 대해 알아보자. 