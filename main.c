/* 
COP 3502C Assignment 3 
This program is written by: Jennifer Nguyen
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "leak_detector_c.h"

// my location global variable
int myX, myY;

struct coordinates
{
    int x;
    int y;
};

void printArray(struct coordinates **q, int size)
{    
    int i;    
    for (i = 0; i < size; i++)        
        printf("%d %d\n", q[i]->x, q[i]->y);    
}
// takes two pointers and returns 1 if ptr1 is closer, -1 if ptr2 is closer
int compareTo(struct coordinates *ptrPT1, struct coordinates *ptrPT2)
{
    double distance1, distance2;
    distance1 = sqrt((pow(myX, 2) + pow(ptrPT1->x, 2)) + (pow(myY, 2) + pow(ptrPT1->y, 2)));
    distance2 = sqrt((pow(myX, 2) + pow(ptrPT2->x, 2)) + (pow(myY, 2) + pow(ptrPT2->y, 2)));
   
   
    // if both points are the same
    if (ptrPT1->x == ptrPT2->x && ptrPT1->y == ptrPT2->y)
        return 0;
    // if ptr1 is closer than ptr2 - neg
    if (distance1 < distance2)
        return -1;
    // if ptr2 is closer than ptr2 - pos
    if (distance1 > distance2)
        return 1;
    // if distance is the same but they are different points
    if (distance1 == distance2)
    {
        if (ptrPT1->x < ptrPT2->x)
            return -2;
        else if (ptrPT1->x > ptrPT2->x)
            return 2;
        else if (ptrPT1->x == ptrPT2->x)
        {
            if (ptrPT1->y < ptrPT2->y)
                return -3;
            if (ptrPT1->y > ptrPT2->y)
                return 3;
        }
    }
}

int binarySearch(struct coordinates **list, int left, int right, struct coordinates *search, int righthold)
{
    int mid, result;
    
    if (left <= right)
    {
        // find midpoint
        mid = (right + left) / 2;
        // to keep in bounds if the search distance is smaller or larger than list
        if (-1 < mid && mid < righthold)
        {
            // use compareTo to search because points are organized by distance
            result = compareTo(search, list[mid]);
            
            if (result < 0)
                return binarySearch(list, left, mid - 1, search, righthold);
            else if (result > 0)
                return binarySearch(list, mid + 1, right, search, righthold);
            // in list return the index
            else if (result == 0)
                return mid;
        }
    }
    // if not in list return -1
    return -1;
}

// search for the queries in the list using binary search
void search(struct coordinates **list, struct coordinates **queries, int listLength, int qLength)
{
    int i, result;
    
    // for each query negative is not found positve is found
    for (i = 0; i < qLength; i++)
    {
        result = binarySearch(list, 0, listLength, queries[i], listLength);
        printf("%d %d ", queries[i]->x, queries[i]->y);
        if (result < 0)
            printf("not found\n");
        else
            printf("found at rank %d\n", result + 1);
    }
   
}

void insertionSort(struct coordinates **list, int length)
{    
    int i, j, result;
  
    for (i = 1; i < length; i++)    
    {         
        // hold key
        struct coordinates *temp = list[i]; 
       
        for (j = i - 1; j >= 0; j--)          
        {              
            // compare the key to all of the points before it
            result = compareTo(list[j], temp);
            
            // if result is positve, needs to move the point until the smaller points are in front of the bigger point
            if (result > 0)
                list[j + 1] = list[j]; 
            else                
                break;  
        }          
        list[j + 1] = temp; 
    }
}

void merge(struct coordinates **list, int left, int mid, int right) 
{    
    int i, j, k, result;    
    int point1 = mid - left + 1;    
    int point2 =  right - mid;    
    struct coordinates **L = (struct coordinates**) malloc(point1 * sizeof(struct coordinates));    
    struct coordinates **R = (struct coordinates**) malloc(point2 * sizeof(struct coordinates));    
  
    // copy data to two arrays
    for (i = 0; i < point1; i++)        
        L[i] = list[left + i];    
        
    for (j = 0; j < point2; j++)        
        R[j] = list[mid + 1+ j];  
      
    i = 0;      
    j = 0;      
    k = left;     
    
    // compare the two arrays & merge
    while (i < point1 && j < point2)    
    {        
        result = compareTo(L[i], R[j]);
        
        if (result <= 0)        
        {            
            list[k] = L[i];            
            i++;        
        }        
        else        
        {            
            list[k] = R[j];            
            j++;        
        }        
        k++;    
        
    }    
    
    // if there is any extra copy elements
    while (i < point1)    
    {        
        list[k] = L[i];        
        i++;        
        k++;    
    } 
    
    while (j < point2)    
    {        
        list[k] = R[j];        
        j++;        
        k++;    
    }
    
    free(L);    
    free(R);
}

void mergeSort(struct coordinates **list, int left, int right, int t)
{    
    if (left < right && right - left > t)    
    { 
        int mid = (left + right) / 2;        
       
        mergeSort(list, left, mid, t);        
        mergeSort(list, mid + 1, right, t);               
        merge(list, left, mid, right);
    }
    else
        insertionSort(list, right);
}

// decides the appropiate sorting method 
void sort(struct coordinates **list, int length, int t)
{
    if (length <= t)
        insertionSort(list, length); 
    else
        mergeSort(list, 0, length, t);
}

// reads input file and returns the points to be searched & queries
struct coordinates** readData(FILE *ifp, int size)
{
    int i;
    
    struct coordinates **data = (struct coordinates**) malloc(size * sizeof(struct coordinates));
    
    for (i = 0; i < size; i++)
    {
        data[i] = (struct coordinates*) malloc(sizeof(struct coordinates));
        fscanf(ifp, "%d", &data[i]->x);
        fscanf(ifp, "%d", &data[i]->y);
    }
    return data;
}

int main()
{
    atexit(report_mem_leak);
    
    int n, s, t;
    
    FILE *ifp, *ofp;
    ifp = fopen("in.txt", "r");
    ofp = fopen("out.txt", "w");
    
    fscanf(ifp, "%d", &myX);
    fscanf(ifp, "%d", &myY);
    fscanf(ifp, "%d", &n);
    fscanf(ifp, "%d", &s);
    fscanf(ifp, "%d", &t);
    
    struct coordinates **list = readData(ifp, n);
    struct coordinates **queries = readData(ifp, s);
    
    sort(list, n, t);
    printArray(list, n);
    search(list, queries, n, s);
    free(list);
    free(queries);
}
