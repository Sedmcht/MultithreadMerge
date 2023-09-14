#include <iostream>
#include <thread>
#include <future>
#include <execution>

#define MAXTHREAD 8

using namespace std;


void merge_(int* arr, int l, int m, int r, int* buff)
{
    int nl = m - l + 1;
    int nr = r - m;

    // создаем временные массивы
    int* left = buff + l;
    int* right = buff + l + nl;

    // копируем данные во временные массивы
    for (int i = 0; i < nl; i++)
        left[i] = arr[l + i];
    for (int j = 0; j < nr; j++)
        right[j] = arr[m + 1 + j];

    int i = 0, j = 0;
    int k = l;  // начало левой части

    while (i < nl && j < nr) {
        // записываем минимальные элементы обратно во входной массив
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        }
        else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }
    // записываем оставшиеся элементы левой части
    while (i < nl) {
        arr[k] = left[i];
        i++;
        k++;
    }
    // записываем оставшиеся элементы правой части
    while (j < nr) {
        arr[k] = right[j];
        j++;
        k++;
    }

}


void mergeSort(int* arr, int l, int r, int* buff)
{
    if (l >= r)
        return;

    int m = (l + r - 1) / 2;
    mergeSort(arr, l, m, buff);
    mergeSort(arr, m + 1, r, buff);
    merge_(arr, l, m, r, buff);
}

void multiMergeSort(int* arr, int size)
{
    int* buff = new int[size + 1];
    const int threadsMax = MAXTHREAD;
    int parts = threadsMax;

    int groups = parts / 2;
    int part = size / parts;
    int start = 0;

    future<void> threads[threadsMax];


    for (int i = 1; i <= parts; i++)
    {
        if (i == parts)
        {
            threads[i - 1] = async(mergeSort, arr, start, size, buff);
        }
        else
        {
            threads[i - 1] = async(mergeSort, arr, start, part * i, buff);
        }
        start = part * i + 1;
    }

    for (int i = 0; i < threadsMax; i++)
    {
        threads[i].wait();
    }

    part *= 2;
    parts /= 2;

    int m = 0;
    for (int j = 1; j <= groups; j++)
    {
        start = 0;
        for (int i = 1; i <= parts; i++)
        {
            if (i == parts)
            {
                m = (part * i - start) / 2 + start;
                threads[i - 1] = async(merge_, arr, start, m, size, buff);
            }
            else
            {
                m = (part * i - start) / 2 + start;
                threads[i - 1] = async(merge_, arr, start, m, part * i, buff);
            }
            start = part * i + 1;
        }

        for (int i = 0; i < parts; i++)
        {
            threads[i].wait();
        }

        parts /= 2;
        part *= 2;

    }

    delete[] buff;
}

int main()
{
    int size = 90000000;
    int* arr = new int[size];
    time_t start, end;

    for (int i = 0; i < size; i++)
        arr[i] = size - i;

    time(&start);
    multiMergeSort(arr, size - 1);
    time(&end);

    cout << "My sort " << difftime(end, start) << endl;

    for (int i = 0; i < size; i++)
        arr[i] = size - i;

    time(&start);
    sort(arr, arr + size);
    time(&end);
    cout << "sort " << difftime(end, start) << endl;

    for (int i = 0; i < size; i++)
        arr[i] = size - i;

    time(&start);
    sort(execution::par, arr, arr + size);
    time(&end);
    cout << "Multithread sort  " << difftime(end, start) << endl;
}