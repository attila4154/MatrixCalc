#pragma once
#include <exception>
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
    WrongDimensions (int lm = -1, int ln = -1, int rm = -1, int rn = -1) : e_Lm (lm), e_Ln (ln),
                                                                           e_Rm (rm), e_Rn (rn) {} 
    void Print (std::ostream & out) {
            out << "Wrong dimensions";
        if (e_Lm == -1) out << '\n';
        else out << ":\nLeft matrix has " << e_Lm << " rows and " << e_Ln << " columns" <<
                           "; right has " << e_Rm << " rows and " << e_Rn << " columns" << std::endl;
    }  

    int e_Lm, e_Ln;
    int e_Rm, e_Rn;
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
