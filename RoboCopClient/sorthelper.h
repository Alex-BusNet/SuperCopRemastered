#ifndef SORTHELPER_H
#define SORTHELPER_H

#include <QVector>
#include "genome.h"

namespace RoboCop
{
    template<class T>
    static int Partition(QVector<T*> &arr, int p, int r, bool (*condition)(T*, T*))
    {
        T* pivot = arr[r];
        int i = p - 1;
        for(int j = p; j < r; j++)
        {
            if((*condition)(arr[j], pivot))
            {
                i++;
                T* temp;
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        arr[r] = arr[i + 1];
        arr[i + 1] = pivot;

        return i+1;
    }

    template<class T>
    static void Quicksort(QVector<T*> &unsorted, int p, int r, bool (*condition)(T*, T*) = [](T *a, T *b){ return (a < b); })
    {
        if(p < r)
        {
            int q = RoboCop::Partition(unsorted, p, r, condition);
            Quicksort(unsorted, p, q - 1, condition);
            Quicksort(unsorted, q + 1, r, condition);
        }
    }



}


#endif // SORTHELPER_H
