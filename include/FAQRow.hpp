#ifndef FAQ_FAQROW_HPP
#define FAQ_FAQROW_HPP
#include <string>
struct FAQRow
{
    unsigned int ROWID;
    std::string QUESTION;
    std::string RESPONSE;
    bool SHOW;
};
#endif
