#include <iostream>
#include <string_view> //for storing path var
#include <fstream>     //for getting access to files
#include <vector>      //storing PT and CT
#include <iterator>    
#include <algorithm>   //reverse() swap()
#include <random>      //key generation
#include <typeinfo>    //debug tool


//TODO task.txt breaks lab (some inputs are not usable due to reinterpret_cast() drawbacks)



template<typename T= uint32_t>
std::vector<T> text_to_arr(std::string_view path, T bitnost)
{
	T var = 0;

	std::fstream text_stream(path.data(), text_stream.binary | text_stream.in | text_stream.out );
	std::vector<T> text_arr(0,0);

	while(text_stream.good())
	{
		text_stream.read(reinterpret_cast<char*>(&var) , sizeof(T));
		text_arr.push_back(var);
	}
	

	//nk
	if(text_arr.size() % 8 != 0)
	{
		while( ( text_arr.size() % 8 ) != 0 )
		{
			text_arr.push_back(text_arr[1]);
		}
	}

	
	text_stream.flush();
	text_stream.close();
	return text_arr;
}

template<typename T= uint32_t>
void arr_to_text(std::string_view path, std::vector<T> text_arr)
{	
	

	std::fstream ts(path.data(), ts.binary | ts.in | ts.out | ts.trunc);
	//nk
	ts.flush();
	
	for(auto it = text_arr.begin(); it != text_arr.end(); ++it)
	{
		ts.write(reinterpret_cast<char*>(&(*it)), sizeof(*it));	
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


template<typename T= uint32_t >
void h(std::vector<T>& P_i)
{
	size_t s = (sizeof(T)*8);
	if(sizeof(T) == 4) 
	{
		P_i[0] += P_i[1];
		P_i[3] ^= P_i[0];
		P_i[3]  = ((P_i[3] << 16) | (P_i[3] >> (s - 16)) );

		P_i[2] += P_i[3];
		P_i[1] ^= P_i[2];
		P_i[1]  = ((P_i[1] << 12) | (P_i[1] >> (s - 12)) );

		P_i[0] += P_i[1];
		P_i[3] ^= P_i[0];
		P_i[3]  = ((P_i[3] << 8 ) | (P_i[3] >> (s - 8)) );

		P_i[2] += P_i[3];
		P_i[1] ^= P_i[2];
		P_i[1]  = ((P_i[1] << 7 ) | (P_i[1] >> (s - 7)) );
	
	}
       	else
	{
		P_i[0] += P_i[1];
		P_i[3] ^= P_i[0];
		P_i[3]  = ((P_i[3] << 16) | (P_i[3] >> (s - 32)) );

		P_i[2] += P_i[3];
		P_i[1] ^= P_i[2];
		P_i[1]  = ((P_i[1] << 12) | (P_i[1] >> (s - 24)) );

		P_i[0] += P_i[1];
		P_i[3] ^= P_i[0];
		P_i[3]  = ((P_i[3] << 8 ) | (P_i[3] >> (s - 16)) );

		P_i[2] += P_i[3];
		P_i[1] ^= P_i[2];
		P_i[1]  = ((P_i[1] << 7 ) | (P_i[1] >> (s - 15)) );
	
	}

	/*
	for(size_t i = 0; i < 4; ++i)
	{
		P_i[index_pair[1].first.first()] += P_i[];
		P_i[] ^= P_i[];
		P_i[]  = (P_i[] << shifts[si]) | (P_i[] >> (s - shifts[si]));
	}

	*/


}


template<typename T= uint32_t>
void encrypt_block(std::vector<std::vector<T>> RK, std::vector<T>& L, std::vector<T>& R )
{
	size_t num_of_iter = 10;
	(sizeof(T) > 4) ? num_of_iter = 14 : num_of_iter = 10;
	
	auto L_holder = L;
	for(size_t i = 0; i < num_of_iter; ++i)
	{
		for(size_t j = 0; j < L.size(); ++j)
		{
			L_holder[j] = L[j];
		}
		for(size_t j = 0; j < L.size(); ++j)
		{
			L[j] ^= RK[i][j];
		}		
			h<T>(L);
			h<T>(L);

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
std::vector<T> encrypt_all(std::vector<std::vector<T>> RK, std::vector<T> text_arr, std::vector<T>& P, 
		           std::vector<T>& L, std::vector<T>& R, bool is_it_decryption = false)
{
	std::vector<T> out_text(0,0);
	while(text_arr.size() != 0)
	{
		preparation(text_arr, P, L, R);
		if(is_it_decryption)
		{
			L.swap(R);
		}
		encrypt_block(RK, L, R);
		
		if(is_it_decryption)
		{
			L.swap(R);
		}

		for(int i = 0; i < L.size(); ++i)
		{
			out_text.push_back(L[i]);
		}
		for(int i = 0; i < R.size(); ++i)
		{
			out_text.push_back(R[i]);
		}
	}

	return out_text;
}

template<typename T= uint32_t>
std::vector<T> decrypt_all(std::vector<std::vector<T>> RK, std::vector<T> text_arr, std::vector<T>& P, std::vector<T>& L, std::vector<T>& R)
{
	std::reverse(RK.begin(), RK.end());
	auto str = encrypt_all(RK, text_arr, P, L, R, true);
	return str;
}


template<typename T= uint32_t>
std::vector<T> GenerateKey(T cipher_bit_length = 256)
{
	std::random_device devUrandom;
	std::vector<T> Key(8,0);
	if(cipher_bit_length == 256)
	{
		std::mt19937_64 generator(devUrandom());
		for(auto it = Key.begin(); it != Key.end(); ++it)
		{
			*it = generator();
		}
	}
	else
	{
		std::mt19937 generator(devUrandom());
		for(auto it = Key.begin(); it != Key.end(); ++it)
		{
			*it = generator();
		}
	}
	
	

	return Key;	
}

template<typename T= uint32_t>
std::vector<T> KeyMenu(T bt)
{
	char answ = 'y';
	
//	std::cout << "Generate Key automatically ? (y/n) //n - you will have to set key using keyboard(0-9) : \n";
//	std::cin >> answ;
	if(answ == 'y')
	{
		auto Key = GenerateKey(bt);
		return Key;
	}
	else
	{
		auto Key = GenerateKey(bt);
		for(auto it = Key.begin(); it != Key.end(); ++it)
		{
			std::cin >> *it;
		}
		return Key;
	}

}


template<typename T= uint32_t>
std::vector<std::vector<T>> RoundKeyGeneration(std::vector<T> K)
{

	T tmv = 0x000F000F;
	std::vector<T> Kl(4,0);
	std::vector<T> Kr(4,0);
	std::vector<T> KI(4,0);
	std::vector<T> KT(4,0);
	std::vector<T> st(4,0);

	T num_of_RK = 0;
	(sizeof(T) > 4) ? num_of_RK = 14 : num_of_RK = 10;
	std::vector<std::vector<T>> RK(num_of_RK, std::vector<T>(4,0));
	for(size_t t = 0; t < (RK.size()/2); ++t) // logic-sife 
	{

		//1 and 2
		for(size_t i = 0 ; i < K.size(); ++i)
		{
			if(i<4){ Kl[i] = K[i]; } else{ Kr[i-4] = K[i]; }
		}
	
		//3
		for(size_t i = 0; i < 4; ++i)
		{
			st[i] = (0x1 ^ Kl[i]);
		}
		h<T>(st);
		h<T>(st);
	
		//4
		for(size_t i = 0; i < 4; ++i)
		{
			st[i] += Kr[i];
		}
		h<T>(st);
		h<T>(st);
	
		//5 and 6 and 7
		for(size_t i = 0; i < 4; ++i)
		{
			KI[i] = (Kl[i] ^ st[i]);
			KT[i] = (KI[i] + tmv);
			st[i] = (Kl[i] + KT[i]);
		}
		h<T>(st);
		h<T>(st);	

		//8
		for(size_t i = 0; i < 4; ++i)
		{
			st[i] = (st[i] ^ KT[i]);
		}
		h<T>(st);
		h<T>(st);
	
		//9
		for(size_t i = 0; i < 4; ++i)
		{
			RK[2*t][i] = (st[i] ^ KT[i]) ;
		}	

		//10
		tmv = (tmv << 1);

		//11 and 12
		for(size_t i = 0; i < 4; ++i)
		{
			KT[i] = (KI[i] + tmv); // maybe KI[i] is really KT[i] here
			st[i] = (Kr[i] + KT[i]);
		}	
		h<T>(st);
		h<T>(st);

		//13
		for(size_t i = 0; i < 4; ++i)
		{
			st[i] = (st[i] ^ KT[i]);
		}
		h<T>(st);
		h<T>(st);

		//14
		for(size_t i = 0; i < 4; ++i)
		{
			RK[(2*t) + 1][i] = (st[i] + KT[i]); //KT here maybe not KT really 
		}

		//15
		tmv = (tmv << 1);

		//16
		T bit = (K[7] >> ((sizeof(T)*8) - 1) );
		for(size_t i = 7; i > 0; --i)
		{ 
			K[i] = ( (K[i] << 1) | (K[i-1] >> ((sizeof(T)*8) - 1) ) );
		}
		K[0] = ( (K[0] << 1) | bit );


	}

	return RK;
}

void first_ten(std::vector<uint32_t> arr)
{
	std::cout << "-------------------------------------------------" << std::endl;
	if(arr.size() > 10)
	{
		for(size_t i = 0 ; i < 10; ++i)
		{
			std::cout << arr[i] << '\n';
		}
	}
	else
	{
		std::cout << "TOO SMALL" << '\n';
	}
	std::cout << "-------------------------------------------------" << std::endl;
}

template<typename T= uint32_t>
void ShowKey(std::vector<T> Key)
{
	std::cout << "Your Key-" << (sizeof(T)*8*8) << " (wordwise in base 10)"  << '\n';
	for(auto it = Key.begin(); it != Key.end(); ++it)
	{
		std::cout << *it << " ";
	}
	std::cout << '\n';
}

int main()
{
	//files
	std::string_view path_in("/home/ftl/kiparys/text");
	std::string_view path_out("/home/ftl/kiparys/t");
	std::string_view path_decr("/home/ftl/kiparys/d");


	char answ = 'y';
//	std::cout << "Do you want to use Kiparys-256 ? (y/n) //n - you will use Kiparys-512 : \n";
//	std::cin >> answ;
	if( answ== 'y')
	{ 
		uint32_t bt = 256; 
		auto text_arr = text_to_arr(path_in, bt);
	//	std::cout << "type of text_arr : " << typeid(text_arr[0]).name() << std::endl;
		std::vector<uint32_t> Key(8,0);
		Key = KeyMenu(bt);
		ShowKey(Key);
		auto RK = RoundKeyGeneration(Key);
		std::vector<uint32_t> P(8,0);
		std::vector<uint32_t> L(4,0);
		std::vector<uint32_t> R(4,0);

		//ENCYPTION
		auto out_text = encrypt_all(RK, text_arr, P, L, R);
		arr_to_text(path_out, out_text);

	
		//DECRYPTION
		auto out_decr = decrypt_all(RK, out_text, P, L, R);
		arr_to_text(path_decr, out_decr);

	}
       	else
	{ 
		uint64_t bt = 512;
		auto text_arr = text_to_arr(path_in, bt);
	//	std::cout << "type of text_arr : " << typeid(text_arr[0]).name() << std::endl;
	
		std::vector<uint64_t> Key(8,0);
		Key = KeyMenu(bt);
		ShowKey(Key);
		auto RK = RoundKeyGeneration(Key);
		std::vector<uint64_t> P(8,0);
		std::vector<uint64_t> L(4,0);
		std::vector<uint64_t> R(4,0);

		//ENCYPTION
		auto out_text = encrypt_all(RK, text_arr, P, L, R);
		arr_to_text(path_out, out_text);

		//DECRYPTION
		auto out_decr = decrypt_all(RK, out_text, P, L, R);
		arr_to_text(path_decr, out_decr);

	}

	//Do not uncomment (debug tool)
	//first_ten(text_arr);
	//first_ten(out_text);
	//first_ten(out_decr);	

return 0;
}

