#ifndef FAQ_MOCKEDDATAACCESS_HPP
#define FAQ_MOCKEDDATAACCESS_HPP
#include "IDataAccess.hpp"
/*
 * Fake Data Access.
 */
class MockedDataAccess : public IDataAccess
{
  public:
    MockedDataAccess()
    {
    }
    virtual bool createQuestion(const std::string &question)
    {
        return false;
    }
    virtual std::optional<std::vector<FAQRow>> getAllValidated()
    {
        FAQRow oneRow;
        oneRow.ROWID = 0;
        oneRow.SHOW = true;
        oneRow.QUESTION = "How old are you?";
        oneRow.RESPONSE = "42";
        FAQRow twoRow;
        twoRow.ROWID = 1;
        twoRow.SHOW = true;
        twoRow.QUESTION = "What time is it?";
        twoRow.RESPONSE = "It's nine o'clock!";
        return {{oneRow, twoRow}};
    }
};
#endif
