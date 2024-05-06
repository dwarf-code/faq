#ifndef FAQ_TOOLS_HPP
#define FAQ_TOOLS_HPP
#include "FAQRow.hpp"
#include <crow.h>
/*
 * Utilitary class.
 */
class Tools
{
  public:
    /*
     * Conversion from FAQRow to wvalue.
     * @param faq : row.
     * @return wvalue corresponding.*/
    static crow::json::wvalue convertToWValue(const FAQRow &faq)
    {
        crow::json::wvalue outValue;
        outValue["rowid"] = faq.ROWID;
        outValue["question"] = faq.QUESTION;
        outValue["response"] = faq.RESPONSE;
        outValue["show"] = faq.SHOW;
        return outValue;
    }
    /*
     * Conversion from vector to list of wvalue. It uses convertToWValue.
     * @param list : liste to convert
     * @return wvalue containing a vector of unitary wvalues.
     */
    template <typename T> static crow::json::wvalue convertListToWValue(const std::vector<T> &list)
    {
        std::vector<crow::json::wvalue> listValue;
        std::for_each(list.begin(), list.end(),
                      [&listValue](const T &value) { listValue.push_back(convertToWValue(value)); });
        return crow::json::wvalue(crow::json::wvalue::list(listValue));
    }
};
#endif
