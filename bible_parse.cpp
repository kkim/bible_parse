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

typedef std::vector<std::string> Chapter;
typedef std::vector<Chapter> Book;
typedef std::map<std::string,int> WordCountVec;
typedef std::map<std::string, Book> Bible;
typedef std::pair<std::string, std::string> BookBook;
typedef std::map<std::tuple<std::string, std::string>, int> Bigram;
typedef std::map<std::vector<std::string>, int> Ngram;

void add_to_ngram(Ngram& ngram, const std::vector<std::string&> wordVec);
void construct_ngram(const Chapter& book, Ngram& ngram);
void construct_ngram(const Book& book, Ngram& ngram);
void construct_ngram(const Bible& bible, Ngram& ngram);

void add_to_bigram(Bigram& bigram, const std::string& word1, const std::string& word2);
void construct_bigram(const Chapter& book, Bigram& bigram);
void construct_bigram(const Book& book, Bigram& bigram);
void construct_bigram(const Bible& bible, Bigram& bigram);


Matrix<double> compute_book_to_book_distance(const std::map<std::string, WordCountVec>& wc_by_book);

template <typename T>
std::map<BookBook, T> BookBookMatrix_to_map(const std::vector<std::string>& booknames, const Matrix<T>& mat);


int count_words(const Chapter& vc, WordCountVec& word_count)
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

int count_words(const Book& vb, WordCountVec& word_count)
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

double wcv_dot_wcv(const WordCountVec& wc1, const WordCountVec& wc2)
{
  double dot = 0.0;
  for(const auto& w_c: wc1)
  {
    // in order to use .at() method, make sure the key exists
    if(wc2.find(w_c.first)!=wc2.end())
    {
      dot += (double)w_c.second*wc2.at(w_c.first);
    }
  }
  return dot;
}

double wcv_distance(const WordCountVec& wc1, const WordCountVec& wc2)
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
  std::sort(word_by_freq.begin(), word_by_freq.end(), [](const word_freq& a, const word_freq& b){return a.second<b.second;});

  for(const auto& wc : word_by_freq)
  {
    std::cout<<wc.first<<":"<<wc.second<<" ";
  }
  std::cout<<std::endl;

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

void add_to_bigram(Bigram& bigram, const std::string& word1, const std::string& word2)
{
  ++bigram[std::make_tuple(word1, word2)];
}

void add_to_bigram(const Chapter& chapter, Bigram& bigram)
{
  std::string word; 
  for(const std::string& verse: chapter)
  {
    std::string word_without_punctuation0;
    for(std::stringstream ss(verse); ss>>word; )
    {
          // Remove punctuation
          std::string word_without_punctuation;
          std::remove_copy_if(word.begin(), word.end(), std::back_inserter(word_without_punctuation), std::ptr_fun<int, int> (&std::ispunct));
          // count
          std::transform(word_without_punctuation.begin(), word_without_punctuation.end(), word_without_punctuation.begin(), ::tolower);

          if(word_without_punctuation0.size()>0)
          {
            add_to_bigram(bigram, word_without_punctuation0, word_without_punctuation);
          }
          word_without_punctuation0 = word_without_punctuation;
    }
  }
}
void add_to_bigram(const Book& book, Bigram& bigram)
{
  for(const Chapter& chapter: book)
  {
    add_to_bigram(chapter, bigram);
  }
}
void construct_bigram(const Bible& bible, Bigram& bigram)
{
  for(const auto& name_book : bible)
  {
    add_to_bigram(name_book.second, bigram);
  }
  
}



Matrix<double> compute_book_to_book_distance(const std::map<std::string, WordCountVec>& wc_by_book)
{
  Matrix<double> dist(wc_by_book.size(), wc_by_book.size());
  int i1 = 0;
  for(auto book1i = wc_by_book.begin(); book1i != wc_by_book.end(); ++book1i, ++i1)
  {
    int i2 = 0;
    for(auto book2i = wc_by_book.begin(); book2i!=book1i; ++book2i, ++i2)
    {
      double d = wcv_distance(wc_by_book.at((*book1i).first), wc_by_book.at((*book2i).first));
      dist[i1][i2] = dist[i2][i1] = d;
    }
    std::cout<<(*book1i).first<<",";
  }
  std::cout<<std::endl;
  std::cout<<dist;
  return dist;
}

template <typename T>
std::map<BookBook, T> BookBookMatrix_to_map(const std::vector<std::string>& booknames, const Matrix<T>& mat)
{
  std::map<BookBook, T> bbdist;

  int i1=0;
  for(std::vector<std::string>::const_iterator book1 = booknames.begin(); book1!=booknames.end(); ++book1)
  {
    std::vector<std::string>::const_iterator book2 = book1; int i2 = i1;
    for(++book2, ++i2; book2!=booknames.end(); ++book2, ++i2)
    {
      bbdist[std::make_pair((*book1), (*book2))] = mat[i1][i2];
    }
    ++i1;
  }

  return bbdist;
}
