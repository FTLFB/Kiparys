#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <iterator>
#include <bitset>
#include <algorithm>

std::vector<size_t> rsh_32{16, 12,  8,  7};
std::vector<size_t> rsh_64{32, 24, 16, 15};

template<typename T= uint32_t>
std::vector<T> text_to_arr(std::string_view path)
{
	T var = 0;

	std::fstream text_stream(path.data(), text_stream.binary | text_stream.in | text_stream.out);
	std::vector<T> text_arr(0,0);


	while(text_stream.good())
	{
		text_stream.read(reinterpret_cast<char*>(&var) , sizeof(T));
		text_arr.push_back(var);
	}
	
	std::cout << "text " << text_arr.size() << '\n';
	
	text_stream.close();
	return text_arr;
}

template<typename T= uint32_t>
void arr_to_text(std::string_view path, std::vector<T> text_arr)
{	
	

	std::fstream ts(path.data(), ts.binary | ts.in | ts.out);

	std::cout << "adsuhhuahdwuhaodwuhaodhaouh!!!!!!!!!!!!!!!!!" << ts.is_open() << std::endl;
	
//	for(auto it = text_arr.begin(); it != text_arr.end(); ++it)
	auto var = text_arr[0];
	for(size_t i = 0; i < text_arr.size(); ++i)
	{
		var = text_arr[i];
		ts.write(reinterpret_cast<char*>(&var), sizeof(var));	
	}

	ts.close();


}


template<typename T= uint32_t>
void preparation(std::vector<T>& text_arr, std::vector<T>& P, std::vector<T>& L, std::vector<T>& R)
{
	if(text_arr.size() == 0)
	{
		return;
	}
		while(text_arr.size() < 8)
		{
			text_arr.push_back(0);
		}		
	
	for(size_t i = 0; i < P.size() ; ++i)
	{
		P[i] = text_arr[i];		
		
		if(i<4)
		{
			L[i] = text_arr[i];
		}
		else
		{
			R[i-4] = text_arr[i];
		}
		
	}


	text_arr.erase(text_arr.begin(), text_arr.begin()+8);

}

template<typename T= uint32_t >//todo shifts array
//TODO pairs array
void h(std::vector<T>& P_i)
{
	size_t s = sizeof(P_i[0])*8;
//	std::cout << s << '\n';

	P_i[0] += P_i[1];
	P_i[3] ^= P_i[0];
	P_i[3]  = (P_i[3] << 16) | (P_i[3] >> (s - 16));

	P_i[2] += P_i[3];
	P_i[1] ^= P_i[2];
	P_i[1]  = (P_i[1] << 12) | (P_i[1] >> (s - 12));

	P_i[0] += P_i[1];
	P_i[3] ^= P_i[0];
	P_i[3]  = (P_i[3] << 8 ) | (P_i[3] >> (s - 8));

	P_i[2] += P_i[3];
	P_i[1] ^= P_i[2];
	P_i[1]  = (P_i[1] << 7 ) | (P_i[1] >> (s - 7));

	/*
	for(size_t si = 0; si < 4; ++si)
	{
		P_i[] += P_i[];
		P_i[] ^= P_i[];
		P_i[]  = (P_i[] << rsh_32[si]) | (P_i[] >> (s - rsh_32[si]));
	}
	*/
}


template<typename T= uint32_t>
void encrypt_block(std::vector<std::vector<T>> RK, std::vector<T>& L, std::vector<T>& R )
{
	size_t num_of_iter = 10;
	(sizeof(L[0]) > 4) ? num_of_iter = 14 : num_of_iter = 10;
	
	auto L_holder = L;
	for(size_t i = 0; i < num_of_iter-1; ++i) //TUT H? skoree vsego uzhe net
	{
		L_holder = L;
		for(size_t j = 0; j < L.size(); ++j)
		{
			L[j] ^= RK[i][j];
			h<T>(L);
			h<T>(L);
		}		

		for(size_t j = 0; j < L.size(); ++j)
		{
			L[j] ^= R[j];
		}

		for(size_t j = 0; j < R.size(); ++j)
		{
			R[j] = L_holder[j];
		}

	}

}




template<typename T= uint32_t>
std::vector<T> encrypt_all(std::vector<std::vector<T>> RK, std::vector<T> text_arr, std::vector<T>& P, std::vector<T>& L, std::vector<T>& R)
{
	std::vector<T> out_text(0,0);
	while(text_arr.size() != 0)
	{
		preparation(text_arr, P, L, R);
		encrypt_block(RK, L, R);
		for(int i = 0; i < L.size(); ++i)
		{
			out_text.push_back(L[i]);
		}
		for(int i = 0; i < R.size(); ++i)
		{
			out_text.push_back(R[i]);
		}
	}

	std::cout << "SHOULD BE " << out_text.size() << std::endl;
	return out_text;
}


template<typename T= uint32_t>
void decrypt_block(std::vector<std::vector<T>> RK, std::vector<T> L, std::vector<T> R)
{
	std::reverse(RK.begin(), RK.end());
	encrypt_block(RK, L, R);
}

template<typename T= uint32_t>
std::vector<T> decrypt_all(std::vector<std::vector<T>> RK, std::vector<T> text_arr, std::vector<T>& P, std::vector<T>& L, std::vector<T>& R)
{
	std::reverse(RK.begin(), RK.end());
	auto str = encrypt_all(RK, text_arr, P, L, R);
	return str;

}


template<typename T= uint32_t>
//64 !!!!!!!!!!!!!
std::vector<std::vector<T>> RoundKeyGeneration()
{
	std::vector<std::vector<T>> RK(10, std::vector<T>(4,0x0F));
	return RK;
}



int main()
{
	std::string_view path_in("/home/ftl/kiparys/text");
	std::string_view path_out("/home/ftl/kiparys/t");
	std::string_view path_decr("/home/ftl/kiparys/d");
	std::string_view pathh("/home/ftl/kiparys/try");

	auto text_arr = text_to_arr(path_in);
	arr_to_text(pathh, text_arr);
	/*
	for(auto it = text_arr.begin(); it != text_arr.end(); ++it)
	{
		std::cout << *it << '\n';
	}
*/

	auto RK = RoundKeyGeneration();

	/*
	for(size_t i = 0; i < RK.size(); ++i)
	{
		for(size_t j = 0; j < RK[i].size(); ++j)
		{
			std::cout << RK[i][j] << ", ";
		}
		std::cout << '\n';
	}
	*/

	std::vector<uint32_t> P(8,0);
	std::vector<uint32_t> L(4,0);
	std::vector<uint32_t> R(4,0);
	




	auto out_text = encrypt_all(RK, text_arr, P, L, R);
	arr_to_text(path_out, out_text);

	//DECRYPTION
	
	auto out_decr = decrypt_all(RK, out_text, P, L, R);
	arr_to_text(path_decr, out_decr);


return 0;
}
