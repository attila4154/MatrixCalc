#pragma once
#include <exception>
//======================================================================
class WrongFormat : public std::exception {
  public:
    const char * what () {
        return "Wrong format\n";
    }
};
//----------------------------------------------------------------------
class WrongDimensions : public std::exception {
  public:
    const char * what () {
        return "Wrong dimensions\n";
    }
};
//----------------------------------------------------------------------
struct exit_exc : public std::exception {
    const char * what () const throw ()
    {
        return "exiting";
    }
};
//----------------------------------------------------------------------
struct wrong_command : public std::exception {
    wrong_command () {text = "";    }
    wrong_command (const std::string & text) : text (text) {}
    const char * what () const throw (){
        if (text == "") return "Wrong command\n";
        else return &text[0];
    }
    
  private:
    std::string text;
};
//======================================================================
struct variable_not_set : public std::exception {
    variable_not_set () {text = "";    }
    variable_not_set (const std::string & text) : text (text) {}
    const char * what () const throw (){
        if (text == "") return "variable not set\n";
        else return &text[0];
    }
    
  private:
    std::string text;
};
//======================================================================
