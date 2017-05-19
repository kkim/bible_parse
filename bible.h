#ifndef _BIBLE_H_
#define _BIBLE_H_
 
typedef std::vector<std::string> Chapter;
typedef std::vector<Chapter> Book;
typedef std::map<std::string, Book> Bible;

Bible KJV();
Bible WEB();

#endif
