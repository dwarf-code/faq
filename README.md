Some weeks ago I was asked if I could make a FAQ (Frequently Asked Questions) for a website. The purpose of a FAQ is to display a set of questions and answers and to let the visitor ask his own questions too.

As there were no limitations regarding the tech stack I could use for that purpose I decided to go for an uncommon combo. Let me go a little bit in details here :

**HTMX** : I used HTMX for the frontend because I was curious about that library, I really appreciate the philosophy behind this technology, it reminds me of the good old days of Javascript / JQuery before the fuss of modern javascripts frameworks.

Unfortunately we won't look at every aspects of HTMX but it will be a good introduction to some main use cases and some specific ones too.

**C++** : I know, I know, C++ has a very pesky reputation these days, and it is not the first language you might think about in this context but with the good set of libraries I wanted to show you that it could be a good choice for this use case.

**Google Spreadsheets** : Google spreadsheets is an interesting collaborative tool that can act as a database for this case, we will see how to use the Google API in order to read and write from and to a spreadsheet.

**Deployment** : We will see how to deploy this solution behind a Nginx https reverse proxy with and without docker.

**Bonuses : Docker, SQLite and hot reload** We might see some alternative workflows and database with SQLite and some hot reload solutions to ease the development process.

```
git clone https://github.com/yhirose/cpp-httplib.git

git clone https://github.com/CrowCpp/Crow.git

git clone https://github.com/nlohmann/json.git

git clone https://github.com/Thalhammer/jwt-cpp.git
```

you can install Crow with those commands :
```
cd lib/Crow
mkdir build && cd build
cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
sudo make install
```

Then we're going to create our CMakeLists.txt as follow :

```
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(faq)

find_package(Crow)
set(CROW_FEATURES "ssl;compression")

find_package(OpenSSL REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/lib")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(faq main.cpp)

target_link_libraries(faq PUBLIC Crow::Crow
  OpenSSL::SSL
  OpenSSL::Crypto
  pthread
  dl)
``` 

and of course a minimal main.cpp with a single Crow route :

```
#include <crow.h>
#include <crow/mustache.h>
int main(int argc, char* argv[])
{
  crow::SimpleApp app;

  CROW_ROUTE(app,"/test")([](){return "ok";});
  app.port(18080).multithreaded().run();
  return 0;
}
```

now we can compile and run the project :

```
cmake .
make
./faq
```

and check with the browser

```
http://localhost:18080/test
```

let's create a minimal template :

```
<html>
  <head>
    <script src="static/lib/htmx.org@1.9.11"></script>
    <title>FAQ</title>
  </head>
  <body>
    <h1>FAQ</h1>
    <button hx-get="/test">Test</button>
  </body>
</html>
```

we need to retrieve htmx minimal js :
```
cd static/lib && wget https://unpkg.com/htmx.org@1.9.11 
```
And update main.cpp
```
#include <crow.h>
#include <crow/mustache.h>
int main(int argc, char *argv[])
{
    crow::SimpleApp app;
    crow::mustache::context ctx;
    auto page = crow::mustache::load("faq.mustache.html");
    CROW_ROUTE(app, "/test")([&page, &ctx]() {return "ok";});
    CROW_ROUTE(app, "/")([&page, &ctx]() { return page.render(ctx); });
    app.port(18080).multithreaded().run();
    return 0;
}
```

Let's check if this works
```
http://localhost:18080/
```

then click on the button the text should change to "ok"

