#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iterator>
#include <regex>
#include <unordered_map>
#include <map>
#include <chrono>
#include <cmath>
#include <queue>
#include "bpMatrix.h"
#include "bpCount.h"
#include <utility> //move

auto t4 = std::chrono::system_clock::now();
auto t5 = std::chrono::system_clock::now();
void tic()
{
  t4 = std::chrono::system_clock::now();
}

void toc()
{
  t5 = std::chrono::system_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t5 - t4);
  std::cout<<"Took "<<time_span.count()<<" second"<<std::endl;
}

inline std::string trim(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

template <typename T, typename Tmetric>
struct map_greater{
  bool operator()(const std::pair<const T, Tmetric>& v1, const std::pair<const T, Tmetric>& v2)const
  {
    return v1.second>v2.second;
  } 
};

std::string print(const std::pair<std::string, std::string>& tss)
{
  std::string s(tss.first+","+tss.second);
  return s;
}

std::string print(const std::string& tss)
{
  return tss;
}

template <typename T, typename Tmetric>
void print_vec_pair_N(const std::vector<std::pair<T, Tmetric> >& v, int N)
{
  int ii = 0;
  for(auto it = v.cbegin(); it!=v.cend() && ii<N; ++it, ++ii)
  {
    std::cout<<print((*it).first)<<":"<<(*it).second<<" ";
    std::cout<<" ";
  }
  std::cout<<std::endl;
}

template<typename T, typename Tmetric>
std::vector<std::pair<const T,Tmetric> > sortedMap(const std::map<T,Tmetric>& m)
{
  typedef std::pair<T, Tmetric> pTi;
  std::vector<pTi> v(m.begin(), m.end());
  
  std::sort(v.begin(), v.end(), [](const pTi& a, const pTi& b){return a.second<b.second;});
  return v;
}

template<typename T, typename Tmetric>
std::vector<std::pair<T,Tmetric> > maxN(const std::map<T,Tmetric>& m, int N)
{
  std::priority_queue<std::pair<T, Tmetric>, std::vector<std::pair<T, Tmetric> >, map_greater<T,Tmetric> > minheap;
  for (auto it: m)
  {
    if(minheap.size()<N)
    {
      minheap.emplace(it);
    }
    else
    {
      if(minheap.top().second<it.second)
      {
        minheap.pop();
        minheap.emplace(it);
      }
    }
  }

  std::vector<std::pair<T,Tmetric> > v;
  while(minheap.size()>0)
  {
    v.emplace_back(minheap.top());
    minheap.pop();
  }
  std::reverse(v.begin(), v.end());
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

  tic();
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
  toc();

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

  print_vec_pair_N(word_by_freq,50);


  // 3. Construct word vectors by book
  std::map<std::string, WordCountVec> wc_by_book;
  for(const auto& book: bible)
  {
    WordCountVec wcv;
    count_words(book.second, wcv);
    wc_by_book[book.first] = wcv;
  }


  // 4. Compute distance matrix between books
  tic();
  Matrix<double> dist = compute_book_to_book_distance(wc_by_book);
  toc();

  // 5. Sort by closest book-book pair
  // - matrix into a vector
  std::vector<std::string> booknames;
  // - - copy only book names
  std::transform(wc_by_book.begin(), wc_by_book.end(), std::back_inserter(booknames), 
                 [](const std::map<std::string, WordCountVec>::value_type& wc){return wc.first;});

  // - - convert it to a book-book to dist map
  std::map<WordWord, double> bbdist = BookBookMatrix_to_map(booknames, dist);

  // - - convert bbdist to a vector of WordWord,double pair
  tic();
  print_vec_pair_N(maxN(bbdist,50),50);
  toc();

  // 6. Find the most frequent bigram
  Bigram bigram;
  construct_bigram(bible, bigram);

  // sort by count
  tic();
  print_vec_pair_N(maxN(bigram,50),50);
  toc();

  std::cout<<"Total "<<word_count.size()<<" words"<<std::endl;
  std::cout<<"Total "<<bigram.size()<<" bigrams"<<std::endl;
  return 0;
}

