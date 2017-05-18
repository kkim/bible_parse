#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iterator>
#include <tuple>
#include <regex>
#include <unordered_map>
#include <map>
#include <chrono>
#include <cmath>
#include <queue>
#include "bpMatrix.h"
#include "bpCount.h"
#include <utility> //move

inline std::string trim(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

template <typename T>
struct map_greater{
  bool operator()(const std::pair<const T, int>& v1, const std::pair<const T, int>& v2)const
  {
    return v1.second>v2.second;
  } 
};


std::string print(const std::tuple<std::string, std::string>& tss)
{
  std::string s(std::get<0>(tss)+","+std::get<1>(tss));
  return s;
}

void print_vec_pair_N(const std::vector<std::pair<std::string, int> >& v, int N)
{
  int ii = 0;
  for(auto it = v.cbegin(); it!=v.cend() && ii<N; ++it, ++ii)
  {
    std::cout<<print((*it).first)<<":"<<(*it).second<<" ";
  }
  std::cout<<std::endl;
}

void print_vec_pair_N(const std::vector<std::pair<const std::tuple<std::string, std::string>, int> >& v, int N)
{
  int ii = 0;
  for(auto it = v.cbegin(); it!=v.cend() && ii<N; ++it, ++ii)
  {
    //std::cout<<print(*it).first<<":"<<(*it).second<<" ";
    std::cout<<" ";
  }
  std::cout<<std::endl;
}
/*
template<typename T>
void print_vec_pair_N(const std::vector<std::pair<const T, int> >& v, int N)
{
  int ii = 0;
  for(auto it = v.cbegin(); it!=v.cend() && ii<N; ++it, ++ii)
  {
    std::cout<<print((*it).first)<<":"<<(*it).second<<" ";
  }
  std::cout<<std::endl;
}*/

template<typename T>
std::vector<std::pair<const T,int> > sortedMap(const std::map<T,int>& m)
{
  typedef std::pair<T, int> pTi;
  std::vector<pTi> v(m.begin(), m.end());
  
  std::sort(v.begin(), v.end(), [](const pTi& a, const pTi& b){return a.second<b.second;});
  return v;
}

template<typename T>
std::vector<std::pair<const T,int> > maxN(const std::map<T,int>& m, int N)
{
  std::priority_queue<std::pair<const T, int>, std::vector<std::pair<const T, int> >, map_greater<T> > minheap;
  for (const auto it: m)
  {
    if(minheap.size()<N)
    {
      minheap.emplace_back(it);
    }
    else
    {
      if(minheap.top().second<it.second)
      {
        minheap.pop();
        minheap.emplace_back(it);
      }
    }
  }

  std::vector<std::pair<T,int> > v;
  while(minheap.size()>0)
  {
    v = std::move(minheap);
  }
  return v;
}


int main(int argc, char** argv)
{
  std::ifstream in("bible.txt");
  std::stringstream sstr;
  std::vector<std::string> verses;
  std::string tmpstr;

  // read file
  std::cout<<"Reading bible"<<std::endl;
  sstr<<in.rdbuf();
  // split into verses
  while(std::getline(sstr, tmpstr, '\n'))
  {
     verses.emplace_back(tmpstr);

     // First token is book name
  }
 
  std::cout<<"Total "<<verses.size()<<" lines"<<std::endl;

  std::regex verse_regex("([1|2|3]? ?[A-Z][a-z]*)\\s+([1-9][0-9]*)\\:([1-9][0-9]*)\\s+(.*)");
  std::smatch verse_match;

  auto t1 = std::chrono::system_clock::now();
  // Make Bible a vector of books
  std::cout<<"Bible with a vector"<<std::endl;
  Bible bible;

  for(const auto &verse : verses)
  { 
    const auto& verset = trim(verse);
    if(std::regex_match(verset, verse_match, verse_regex))
    {
      // Using regex, extract book name, chapter#, verse#, and the verse line.
      std::string bookname = *(verse_match.begin()+1);
      int chapter_n = std::stoi(*(verse_match.begin()+2));
      int verse_n = std::stoi(*(verse_match.begin()+3));
      std::string verse_line =*(verse_match.begin()+4);
     
      if(bible.find(bookname) == bible.end())
      {
        bible[bookname] = Book(1, Chapter(1,""));
      }
      if(bible[bookname].size()==chapter_n)
      {
        bible[bookname].emplace_back(Chapter(1,""));
      }
      bible[bookname][chapter_n].emplace_back(verse_line);
    }
  }
  auto t2 = std::chrono::system_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  std::cout<<"Took "<<time_span.count()<<" second"<<std::endl;

  std::cout<<bible["John"][3][16]<<std::endl;

  // 1. Word count
  WordCountVec word_count;
  int n_word = 0;

  n_word = count_words(bible, word_count);

#ifdef PRINT_WORDS_BY_COUNT
  for(const auto& wc : word_count)
  {
    std::cout<<wc.first<<" : "<<wc.second<<" ";
  }
#endif

  std::cout<<"Total "<<n_word<<" words"<<std::endl;

  // 2. Sort words by counts
  typedef std::pair<std::string, int> word_freq;
  std::vector<word_freq> word_by_freq(word_count.begin(), word_count.end());
  std::sort(word_by_freq.begin(), word_by_freq.end(), [](const word_freq& a, const word_freq& b){return a.second>b.second;});

/*
  for(const auto& wc : word_by_freq)
  {
    std::cout<<wc.first<<":"<<wc.second<<" ";
  }
  std::cout<<std::endl;
*/
  print_vec_pair_N(word_by_freq,10);

  return 0;

  // 3. Construct word vectors by book
  std::map<std::string, WordCountVec> wc_by_book;
  for(const auto& book: bible)
  {
    WordCountVec wcv;
    count_words(book.second, wcv);
    wc_by_book[book.first] = wcv;
  }


  // 4. Compute distance matrix between books
  auto t4 = std::chrono::system_clock::now();
  Matrix<double> dist;
  dist = compute_book_to_book_distance(wc_by_book);
  auto t5 = std::chrono::system_clock::now();
  time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t5 - t4);
  std::cout<<"Took "<<time_span.count()<<" second"<<std::endl;


  // 5. Sort by closest book-book pair
  // - matrix into a vector
  std::vector<std::string> booknames;
  // - - copy only book names
  std::transform(wc_by_book.begin(), wc_by_book.end(), std::back_inserter(booknames), 
                 [](const std::map<std::string, WordCountVec>::value_type& wc){return wc.first;});

  // - - convert it to a book-book to dist map
  std::map<BookBook, double> bbdist = BookBookMatrix_to_map(booknames, dist);

  // - - convert bbdist to a vector of BookBook,double pair
  std::vector<std::pair<BookBook, double> > vbbdist;
  std::transform(bbdist.begin(), bbdist.end(), std::back_inserter(vbbdist),[](const std::map<BookBook,double>::value_type& bbd){return std::make_pair(bbd.first, bbd.second);});

  std::sort(vbbdist.begin(), vbbdist.end(), [](const std::pair<BookBook,double>& bbd1, const std::pair<BookBook, double>& bbd2)
  {return bbd1.second<bbd2.second;});

  for(auto bbd: vbbdist)
  {
    std::cout<<bbd.first.first<<"-"<<bbd.first.second<<": "<<bbd.second<<std::endl;
  } 


  // 6. Find the most frequent bigram
  Bigram bigram;
  construct_bigram(bible, bigram);
  // sort by count
  typedef std::pair<std::tuple<std::string, std::string>, int> bigram_freq;
  std::vector<bigram_freq> bigram_by_freq(bigram.begin(), bigram.end());
  
  std::sort(bigram_by_freq.begin(), bigram_by_freq.end(), [](const bigram_freq& a, const bigram_freq& b){return a.second<b.second;});

  for(const auto& bc : bigram_by_freq)
  {
    std::cout<<std::get<0>(bc.first)<<","<<std::get<1>(bc.first)<<":"<<bc.second<<" ";
  }
  std::cout<<std::endl;


  std::cout<<"Total "<<word_count.size()<<" words"<<std::endl;
  std::cout<<"Total "<<bigram.size()<<" bigrams"<<std::endl;
  return 0;
}

