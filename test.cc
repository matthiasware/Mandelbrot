#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

template<typename T>
void print_vector(const std::vector<T> &v)
{
	for_each(std::begin(v), std::end(v), [&v](T x){std::cout << x << std::endl;});
}


std::vector<int> f(int n, int add)
{
	std::vector<int> result;
	for(int i=0; i<n; i++)
	{
		result.push_back(i+add);
	}
	return result;
}


std::vector<int> f2(int n, int add)
{
	std::vector<int> result;
	result.reserve(2*n);
	for(int i=0; i<n; i++)
	{
		result.push_back(i+add);
	}
	return result;
}

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> uni(0,1000);

	std::uniform_int_distribution<int> uni2(10'000'000, 20'000'000);

	int n=uni2(gen);
	int add = uni(gen);

	auto start = std::chrono::high_resolution_clock::now();
	f(n, add);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	std::cout << diff.count() << std::endl;

	start = std::chrono::high_resolution_clock::now();
	f2(n, add);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	std::cout << diff.count() << std::endl;
}	
