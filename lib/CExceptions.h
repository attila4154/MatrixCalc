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
        if (varName == "") return "variable not set\n";
        else {
            // std::string text = "variable \'";
            // text += varName.data();
            // text.reserve (100);
            // text = text + "\' not set\n";
            // return text.data();
            return "variable not set\n";
        }
    }
    
  private:
    std::string varName;
};
//======================================================================
