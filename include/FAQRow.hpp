#ifndef FAQ_FAQROW_HPP
#define FAQ_FAQROW_HPP
#include <string>
#include <iostream>
struct FAQRow
{
    unsigned int ROWID;
    std::string QUESTION;
    std::string RESPONSE;
    bool SHOW;
    friend std::ostream &operator<<(std::ostream &os, const FAQRow &row)
    {
        os << "ROWID : " << row.ROWID << " QUESTION : " << row.QUESTION << " RESPONSE : " << row.RESPONSE
           << " SHOW : " << row.SHOW;
        return os;
    }
};
#endif
