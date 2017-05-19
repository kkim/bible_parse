#include <fstream>
#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <bible.h>


inline std::string trim(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

Bible WEB()
{
  std::ifstream in("world_english_bible.txt");
  std::stringstream sstr;
  std::vector<std::string> lines;
  std::string tmpstr;
  Bible bible;

  sstr<<in.rdbuf();
  // Find if next pattern is book name, or chater:verse, or none of these
  while(std::getline(sstr, tmpstr, '\n'))
  {
     lines.emplace_back(tmpstr);
     // First token is book name
  }


  std::regex Book_regex("Book [0-9][0-9] ([1|2|3]? ?[A-Z][a-z]*)");
  std::regex verse_regex("([01][0-9][0-9]):([01][0-9][0-9])\\s+(.*)");
  std::smatch verse_match;

  
  std::string bookname;
  int chapter_n, verse_n;
  for(const auto &line : lines)
  { 
    const auto& linet = trim(line);
    if(std::regex_match(linet, verse_match, Book_regex))
    {
      // New book added
      bookname = *(verse_match.begin()+1);
      bible[bookname] = Book(1, Chapter(1,""));
    }
    else if(std::regex_match(linet, verse_match, verse_regex))
    { // New verse found
      chapter_n = std::stoi(*(verse_match.begin()+1));
      verse_n = std::stoi(*(verse_match.begin()+2));
      std::string verse_line =*(verse_match.begin()+3);

      if(verse_n==1)
      {
        // Add a new chapter
        bible[bookname].emplace_back(Chapter(1,""));
      }
      bible[bookname][chapter_n].emplace_back(verse_line);
    }
    else if (!(bible[bookname].empty()) && !(bible[bookname][chapter_n].empty()))
    {
      std::string curr = bible[bookname][chapter_n][verse_n];
      std::string verse_line =linet;
      bible[bookname][chapter_n][verse_n]=curr+" "+linet;
    }
  }
  return bible;
}

Bible KJV()
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

  return bible;
}


