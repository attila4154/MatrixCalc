#pragma once
#include <exception>
#include <string>
#include <iostream>
//======================================================================
class WrongFormat : public std::exception {
  public:
    WrongFormat (std::string && text = "") : text (text) {}  
    const char * what () {
        if (text.empty())
            return "Wrong format\n";
        else return text.data();
    }

  private:
    std::string text;
};
//----------------------------------------------------------------------
class WrongDimensions : public std::exception {
  public:
    const char * what () {return "Wrong dimensions\n";}
};
//----------------------------------------------------------------------
struct exit_exc : public std::exception {
};
//----------------------------------------------------------------------
struct wrong_command : public std::exception {
    wrong_command () {text = "";    }
    wrong_command (const std::string & text) : text (text) {}
    const char * what () const throw (){
        if (text == "") return "Wrong command\n";
        else return text.data();
    }
    void Print (std::ostream & out) {
        if (text != "")
            out << "Unrecognized command \'" << text << '\'' << std::endl;
        else out << what ();
    }
    
  private:
    std::string text;
};
//======================================================================
struct variable_not_set : public std::exception {
    variable_not_set () {varName = "";    }
    variable_not_set (const std::string & var) : varName (var) {}
    const char * what () const throw (){
        return varName.data();
    }
    
  private:
    std::string varName;
};
//======================================================================
