#ifndef FAQ_GOOGLESHEETDATAACCESS_HPP
#define FAQ_GOOGLESHEETDATAACCESS_HPP
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "FAQRow.hpp"
#include "IDataAccess.hpp"
#include "Tools.hpp"
#include "cpp-httplib/httplib.h"
#include "json/single_include/nlohmann/json.hpp"
#include <iostream>
using json = nlohmann::json;
class GoogleSheetDataAccess : public IDataAccess
{
  public:
    GoogleSheetDataAccess(const std::string &pSpreadsheetId, const std::string &pApiKey, const std::string &pTab,
                          const std::string &pPrivateKey, const std::string &pServiceAccount)
        : mSpreadsheetId(pSpreadsheetId), mApiKey(pApiKey), mTab(pTab), mPrivateKey(pPrivateKey),
          mServiceAccount(pServiceAccount)
    {
    }
    virtual bool createQuestion(const std::string &question)
    {
        majAccessToken();

        unsigned int numQuestion = Tools::randomUInt();
        json newQuestion;
        // we specify our range
        newQuestion["range"] = mTab + "!A1:D1";
        // we specify our dimension (here we add a row)
        newQuestion["majorDimension"] = "ROWS";
        // Here is the new row content.
        newQuestion["values"][0] = json::array({numQuestion, question, "", "FALSE"});
        std::cout << "New question with : " << newQuestion.dump() << std::endl;
        const httplib::Headers headers = {{"Content-Type", "application/json"},
                                          {"Authorization", "Bearer " + mAccessToken}};

        auto res = mHttpClient.Post("/v4/spreadsheets/" + mSpreadsheetId + "/values/" + mTab + "!A1:D1" +
                                        ":append?valueInputOption=RAW&insertDataOption=INSERT_ROWS",
                                    headers, newQuestion.dump(), "application/json");
        std::cout << res->status << std::endl;
        std::cout << res->reason << std::endl;
        // We return true if status is 200 ok and false otherwise.
        return (res->status == 200);
    }
    /*
     * Method for updating access token.
     */
    /* Method for getting all validated (shown) questions.
     * @return optional with all questions/answers
     */
    virtual std::optional<std::vector<FAQRow>> getAllValidated()
    {
        auto res = mHttpClient.Get("/v4/spreadsheets/" + mSpreadsheetId + "/values:batchGet?ranges=" + mTab +
                                   "&key=" + mApiKey);

        if (!res->body.empty())
        {
            std::cout << res->body << std::endl;

            std::vector<FAQRow> returnValue;

            json sheet = json::parse(res->body);

            auto lines = sheet["valueRanges"][0]["values"];

            if (!lines.empty())
            {
                lines.erase(lines.begin());

                for (auto &line : lines)
                {
                    bool show = line[3] == "TRUE" ? true : false;
                    if (show)
                    {
                        try
                        {
                            FAQRow row;
                            std::string rowidstr = line[0];

                            row.ROWID = std::stoul(rowidstr);
                            row.QUESTION = line[1];
                            row.RESPONSE = line[2];
                            row.SHOW = true;

                            returnValue.push_back(row);
                        }
                        catch (const std::invalid_argument &e)
                        {
                            std::cout << "Cannot instantiate FAQRow : " << e.what() << " : " << line[0] << "\n";
                        }
                        catch (const std::out_of_range &e)
                        {
                            std::cout << "Cannot instantiate FAQRow : " << e.what() << " : " << line[0] << "\n";
                        }
                    }
                }
                return {returnValue};
            }
        }
        return std::nullopt;
    }

  private:
    /*
     * Method for updating access token.
     */
    void majAccessToken()
    {
        // If token is invalid or absent.
        if (Tools::currentTimestamp() >= mAccessTokenTimestamp || mAccessToken.empty())
        {
            // we create a new JWT token.
            std::string token = Tools::JWTToken(mServiceAccount, "https://www.googleapis.com/auth/spreadsheets",
                                                "https://oauth2.googleapis.com/token", mPrivateKey);
            std::cout << "Token JWT : " << token << std::endl;
            // we call the api to get a new access token from JWT token.
            auto res =
                mHttpClient.Post("https://oauth2.googleapis.com/"
                                 "token?grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=" +
                                 token);
            if (res->status == 200 && !res->body.empty())
            {
                // retrieve access token from body response.
                mAccessToken = json::parse(res->body)["access_token"];
                // we update timestamp validity
                mAccessTokenTimestamp = Tools::currentTimestamp(std::chrono::minutes(30));
            }
        }
    }
    httplib::Client mHttpClient{"https://sheets.googleapis.com"};
    std::string mSpreadsheetId;
    std::string mTab;
    std::string mApiKey;
    std::string mPrivateKey;
    std::string mServiceAccount;
    int64_t mAccessTokenTimestamp{0};
    std::string mAccessToken;
};
#endif
