#include <bpCount.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>

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

