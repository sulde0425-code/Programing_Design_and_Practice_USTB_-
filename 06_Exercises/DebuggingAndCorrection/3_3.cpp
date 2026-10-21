#include <iostream>
#include <vector>

const int maxn = 5000;
int a[maxn];

// 交换函数，利用异或操作的可逆性
void swap(int& a, int& b) 
{
	int temp;
    temp = b;
    b = a;
    a = temp;
}

int main(int argc, char *argv[]) 
{
    int n;
    std::cin >> n;
    if (n <= 0 || n > maxn) 
    {
        std::cout << "Invalid input array length:\n"
            "   must be in the range of 1 to "
            << maxn << std::endl;
    }

    for (int i = 0; i < n; ++i) 
    {
        std::cin >> a[i];
    }

    // 冒泡排序
    
    for (int i = n - 1; i > 0; --i) 
    {
        for(int j=0;j<i;j++){
			
            if (a[j] > a[i])
            {
                swap(a[j], a[i]);
            }
        }
    }

    for (int i = 0; i < n; ++i)
    {
        std::cout << a[i] << (i < n - 1 ? ' ' : '\n');
    }
    return 0;
}