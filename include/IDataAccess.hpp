#ifndef FAQ_IDATAACCESS_HPP
#define FAQ_IDATAACCESS_HPP
#include "FAQRow.hpp"
#include <optional>
#include <vector>
/*
 * Interface for data access
 */
class IDataAccess
{
  public:
    /*
     * Method for creating a new question
     * @param question : question content.
     * @return true if question created, false otherwise.
     */
    virtual bool createQuestion(const std::string &question) = 0;
    /* Method for getting all validated (shown) questions.
     * @return optional with all questions/answers
     */
    virtual std::optional<std::vector<FAQRow>> getAllValidated() = 0;
    virtual ~IDataAccess()
    {
    }
};
#endif
