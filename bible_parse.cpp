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

template<typename T>
class Matrix:public std::vector<std::vector<T> >
{
  friend std::ostream &operator<<(std::ostream& os, const Matrix<T>& m)
  {
    for(const auto &row: m)
    {
      for(const auto& col: row)
      {
        os<<col<<",";
      }
      os<<std::endl;
    }
    
    return os;
  }

  public:
  Matrix(){}
 ~Matrix(){}
  Matrix(size_t n_row, size_t n_col)
  {
    this->resize(n_row, n_col);
  }


  void resize(size_t n_row, size_t n_col)
  {
    (dynamic_cast<std::vector<std::vector<T> >*>(this))->resize(n_row);
    for(auto &row: *this)
    {
      row.resize(n_col);
    }
  }

  void set_val(T v)
  {
    for(auto &row: *this)
    {
      for(auto& col: row)
      {
        col = v;
      }
    }
  }

};

inline std::string trim(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

typedef std::vector<std::string> vchapter;
typedef std::vector<vchapter> vbook;
typedef std::map<std::string,int> WordCountVec;
typedef std::map<std::string, vbook> Bible;

int count_words(const vchapter& vc, WordCountVec& word_count)
{
  std::string word;
  int n_word = 0;
  for(const auto& verse: vc)
  {
    for(std::stringstream ss(verse); ss>>word; )
    {
          // Remove punctuation
          std::string word_without_punctuation;
          std::remove_copy_if(word.begin(), word.end(), std::back_inserter(word_without_punctuation), std::ptr_fun<int, int> (&std::ispunct));
          // count
          std::transform(word_without_punctuation.begin(), word_without_punctuation.end(), word_without_punctuation.begin(), ::tolower);
          word_count[word_without_punctuation]++;
          ++n_word;
    }
  }
  return n_word;
}

int count_words(const vbook& vb, WordCountVec& word_count)
{
  int n_word = 0;
  for(const auto& chapter: vb)
  {
    n_word += count_words(chapter, word_count);
  }
  return n_word; 
}

int count_words(const Bible& bible, WordCountVec& word_count)
{
  int n_word = 0;
  for(const auto& book: bible)
  {
    n_word += count_words(book.second, word_count);
  }
  return n_word;
}

double wcv_length(const WordCountVec& wc)
{
  double mag = 0.0;
  for(const auto w_c: wc)
  {
    mag += (double)w_c.second*w_c.second;
  }
  return sqrt(mag);
}

double wcv_dot_wcv(WordCountVec& wc1, WordCountVec& wc2)
{
  double dot = 0.0;
  for(const auto w_c: wc1)
  {
    dot += (double)w_c.second*wc2[w_c.first];
  }
  return dot;
}

double wcv_distance(WordCountVec& wc1, WordCountVec& wc2)
{
  double mag1 = wcv_length(wc1);
  double mag2 = wcv_length(wc2);

  if(mag1*mag2==0.0) return 0.0;
  else return wcv_dot_wcv(wc1, wc2)/(mag1*mag2);
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


#ifdef USE_MAP
  // Make Bible with a map
  std::cout<<"Bible with a map"<<std::endl;
  auto t0 = std::chrono::system_clock::now();

  typedef std::map<int, std::string> chapter;
  typedef std::map<int, chapter> book;
  std::map<std::string, book> bible;

  for(const auto &verse : verses)
  { 
    const auto& verset = trim(verse);
    if(std::regex_match(verset, verse_match, verse_regex))
    {
      std::string bookname = *(verse_match.begin()+1);
      int chapter_n = std::stoi(*(verse_match.begin()+2));
      int verse_n = std::stoi(*(verse_match.begin()+3));
      std::string verse_line =*(verse_match.begin()+4);
     
      if(bible.find(bookname) == bible.end())
      {
        bible[bookname] = book();
      }
      if(bible[bookname].find(chapter_n) ==bible[bookname].end())
      {
        bible[bookname][chapter_n] = chapter();
      }
      bible[bookname][chapter_n][verse_n] = verse_line;

    }
  }
#endif

#ifdef USE_MAP
  auto t3 = std::chrono::system_clock::now();
  std::cout<<"Took "<<(t3-t0).count()<<" second"<<std::endl;
  std::cout<<bible["John"][3][16]<<std::endl;
#endif

  auto t1 = std::chrono::system_clock::now();
  // Make Bible with a vector
  std::cout<<"Bible with a vector"<<std::endl;
  Bible vbible;

  for(const auto &verse : verses)
  { 
    const auto& verset = trim(verse);
    if(std::regex_match(verset, verse_match, verse_regex))
    {
      std::string bookname = *(verse_match.begin()+1);
      int chapter_n = std::stoi(*(verse_match.begin()+2));
      int verse_n = std::stoi(*(verse_match.begin()+3));
      std::string verse_line =*(verse_match.begin()+4);
     
      if(vbible.find(bookname) == vbible.end())
      {
        vbible[bookname] = vbook(1, vchapter(1,""));
      }
      if(vbible[bookname].size()==chapter_n)
      {
        vbible[bookname].emplace_back(vchapter(1,""));
      }

      vbible[bookname][chapter_n].emplace_back(verse_line);

    }
  }
  auto t2 = std::chrono::system_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  std::cout<<"Took "<<time_span.count()<<" second"<<std::endl;

  std::cout<<vbible["John"][3][16]<<std::endl;

  // Word count
  WordCountVec word_count;
  int n_word = 0;

  n_word = count_words(vbible, word_count);

#ifdef NO_USE_FUNC
  for(const auto& bookname_book: vbible)
  {
    std::cout<<bookname_book.first<<std::endl;
    for(const auto& chapter: bookname_book.second)
    {
      std::cout<<chapter.size()<<" ";
      for(const auto& verse: chapter)
      {
        std::string word;
        for(std::stringstream ss(verse); ss>>word; )
        {
          // Remove punctuation
          std::string word_without_punctuation;
          std::remove_copy_if(word.begin(), word.end(), std::back_inserter(word_without_punctuation), std::ptr_fun<int, int> (&std::ispunct));
          // count
          std::transform(word_without_punctuation.begin(), word_without_punctuation.end(), word_without_punctuation.begin(), ::tolower);
          word_count[word_without_punctuation]++;
          ++n_word;
        }
      }
    }
    //std::cout<<std::endl;
  }
#endif

#ifdef PRINT_WORDS_BY_COUNT
  for(const auto& wc : word_count)
  {
    std::cout<<wc.first<<" : "<<wc.second<<" ";
  }
#endif

  std::cout<<"Total "<<n_word<<" words"<<std::endl;

  // Sort words by counts
  typedef std::pair<std::string, int> word_freq;
  std::vector<word_freq> word_by_freq(word_count.begin(), word_count.end());
  std::sort(word_by_freq.begin(), word_by_freq.end(), [](const word_freq& a, const word_freq& b){return a.second<b.second;});

  for(const auto& wc : word_by_freq)
  {
    std::cout<<wc.first<<":"<<wc.second<<" ";
  }
  std::cout<<std::endl;

  // Construct word vectors by book
  std::map<std::string, WordCountVec> wc_by_book;
  for(const auto& book: vbible)
  {
    WordCountVec wcv;
    count_words(book.second, wcv);
    wc_by_book[book.first] = wcv;
  }

  // Compute distance
  auto t4 = std::chrono::system_clock::now();
  Matrix<double> dist(vbible.size(), vbible.size());
  typedef std::pair<std::string, std::string> BookBook;
  std::map<BookBook, double> bbdist;

  int i1 = 0;
  for(auto book1i = vbible.begin(); book1i != vbible.end(); ++book1i, ++i1)
  {
    auto book2i = book1i; int i2 = i1;
    for(++book2i, ++i2; book2i!=vbible.end(); ++book2i, ++i2)
    {
      double d = wcv_distance(wc_by_book[(*book1i).first], wc_by_book[(*book2i).first]);
      dist[i1][i2] = dist[i2][i1] = d;

      bbdist[std::make_pair((*book1i).first, (*book2i).first)] = d;
    }
    std::cout<<(*book1i).first<<",";
  }
  std::cout<<std::endl;
  std::cout<<dist;
  auto t5 = std::chrono::system_clock::now();
  time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t5 - t4);
  std::cout<<"Took "<<time_span.count()<<" second"<<std::endl;

  // Sort by closest book-book pair
  std::vector<std::pair<BookBook, double> > vbbdist(bbdist.begin(), bbdist.end());
  std::sort(vbbdist.begin(), vbbdist.end(), [](const std::pair<BookBook,double>& bbd1, const std::pair<BookBook, double>& bbd2)
  {return bbd1.second<bbd2.second;});
  for(auto bbd: vbbdist)
  {
    std::cout<<bbd.first.first<<"-"<<bbd.first.second<<": "<<bbd.second<<std::endl;
  } 

  return 0;
}
