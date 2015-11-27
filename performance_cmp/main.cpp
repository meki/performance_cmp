#include <vector>


#include <map>

#include <unordered_map>

#include <string>
#include <random>

std::wstring generateRandomString(std::mt19937& engine)
{
	static const int LEN_MIN = 5;
	static const int LEN_MAX = 100;
	static const int WORDS = 36;
	static const wchar_t res[WORDS + 1] = L"abcdefghijklmnopqrstuvwxyz‚ ‚¢‚¤‚¦‚¨‚©‚«‚­‚¯‚±";
	static std::uniform_int_distribution<int> distWord(0, WORDS - 1);

	std::uniform_int_distribution<int> distLength(LEN_MIN, LEN_MAX);

	wchar_t o[LEN_MAX + 1];
	size_t length = distLength(engine);
	for (size_t i = 0; i < length; ++i)
	{
		o[i] = res[distWord(engine)];
	}
	o[length] = L'\0';

	return o;
}

#include <chrono>
#include <iostream>
using namespace std;

#include <windows.h>

template <typename Container>
std::pair<size_t, size_t> perfCheck(size_t elementCount, size_t accessCount)
{
	Container cont;
	std::vector<wstring> keys;
	std::mt19937 engine(13354);

	for (size_t i = 0; i < elementCount; ++i)
	{
		wstring key = generateRandomString(engine);
		keys.push_back(key);
		cont[key] = generateRandomString(engine);
	}

	std::uniform_int_distribution<size_t> idx(0, elementCount - 1);

	auto start = std::chrono::high_resolution_clock::now();

	size_t count = 0;
	for (size_t i = 0; i < accessCount; ++i)
	{
		auto it = cont.find(keys[idx(engine)]);
		count += it->second.size();

		// wcout << it->first << " ||| " << it->second << endl;
	}

	auto duration = std::chrono::high_resolution_clock::now() - start;

	size_t micros = chrono::duration_cast <chrono::milliseconds>(duration).count();

	return make_pair(micros, count);
}

#include <algorithm>

std::pair<size_t, size_t> perfCheckVect(size_t elementCount, size_t accessCount)
{
	using Pair = std::pair<wstring, wstring>;
	using SortedVector = std::vector <Pair>;
	SortedVector cont;
	std::vector<wstring> keys;
	std::mt19937 engine(13354);

	for (size_t i = 0; i < elementCount; ++i)
	{
		wstring key = generateRandomString(engine);
		keys.push_back(key);
		cont.push_back(make_pair(key, generateRandomString(engine)));
	}

	auto less = [](const Pair& l, const Pair& r) {
		return l.first < r.first;
	};

	std::sort(cont.begin(), cont.end(), less);
	cont.shrink_to_fit();

	std::uniform_int_distribution<size_t> idx(0, elementCount - 1);

	auto start = std::chrono::high_resolution_clock::now();

	auto finder = [](const Pair& p, const wstring& str) {
		return p.first < str;
	};

	size_t count = 0;
	for (size_t i = 0; i < accessCount; ++i)
	{
		auto it = std::lower_bound(cont.begin(), cont.end(), keys[idx(engine)], finder);
		count += it->second.size();

		// wcout << it->first << " ||| " << it->second << endl;
	}

	auto duration = std::chrono::high_resolution_clock::now() - start;

	size_t micros = chrono::duration_cast <chrono::milliseconds>(duration).count();

	return make_pair(micros, count);
}

std::pair<size_t, size_t> perfCheckUnsortVect(size_t elementCount, size_t accessCount)
{
	using Pair = std::pair<wstring, wstring>;
	using SortedVector = std::vector <Pair>;
	SortedVector cont;
	std::vector<wstring> keys;
	std::mt19937 engine(13354);

	for (size_t i = 0; i < elementCount; ++i)
	{
		wstring key = generateRandomString(engine);
		keys.push_back(key);
		cont.push_back(make_pair(key, generateRandomString(engine)));
	}

	std::uniform_int_distribution<size_t> idx(0, elementCount - 1);

	auto start = std::chrono::high_resolution_clock::now();

	wstring target;
	auto finder = [&](const Pair& p) {
		return p.first == target;
	};

	size_t count = 0;
	for (size_t i = 0; i < accessCount; ++i)
	{
		target = keys[idx(engine)];
		auto it = std::find_if(cont.begin(), cont.end(), finder);
		
		count += it->second.size();
		// wcout << it->first << " ||| " << it->second << endl;
	}

	auto duration = std::chrono::high_resolution_clock::now() - start;

	size_t micros = chrono::duration_cast <chrono::milliseconds>(duration).count();

	return make_pair(micros, count);
}

int main(int argc, char** argv)
{
// 	std::vector<int> v = { 1,2,3,4,5,6,6,7,8,9 };
// 	auto it = std::lower_bound(v.begin(), v.end(), 6, [](int i, int j) {
// 		return i < j;
// 	});
// 
// 	for (; it != v.end(); ++it) { cout << *it << endl; }
// 
// 	return 0;

	auto start = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::high_resolution_clock::now() - start;

	auto count = std::chrono::duration_cast < std::chrono::microseconds > (duration).count();

	wcout.imbue(std::locale(""));

	const int elems = 800;
	const int access = 5000000;

	auto chkUo = perfCheck < std::unordered_map<wstring, wstring>>(elems, access);
	cout << "unordered_map: " << chkUo.first << "(msec), " << chkUo.second << endl;

	auto chkMap = perfCheck < std::map<wstring, wstring>>(elems, access);
	cout << "map: " << chkMap.first << "(msec), " << chkMap.second << endl;

	auto chkVec = perfCheckVect(elems, access);
	cout << "sorted_vector: " << chkVec.first << "(msec), " << chkVec.second << endl;

	auto chkUnso = perfCheckUnsortVect(elems, access);
	cout << "unsorted_sorted_vector: " << chkUnso.first << "(msec), " << chkUnso.second << endl;

	if (!(chkUo.second == chkMap.second && chkMap.second == chkVec.second))
	{
		cout << "error!!!!" << endl;
	}

	return 0;
}