#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <bpMatrix.h>

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


int count_words(const Chapter& vc, WordCountVec& word_count);
int count_words(const Book& vb, WordCountVec& word_count);
int count_words(const Bible& bible, WordCountVec& word_count);


double wcv_length(const WordCountVec& wc);
double wcv_dot_wcv(const WordCountVec& wc1, const WordCountVec& wc2);
double wcv_distance(const WordCountVec& wc1, const WordCountVec& wc2);



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
