#pragma once

// #include "../lib/CDense.h"
CDense::CDense               (int m, int n) : CMatrix (m,n) {
    m_matrix.resize(m);
    m_matrix.reserve(m);
    // for (int i = 0; i < m_m; i++)
    //     m_matrix[i].reserve(m_n);
}
CDense::CDense               () : CMatrix (0,0) {
    // m_matrix.resize(m_m);
    // m_matrix.reserve(m_m);
}
CDense::CDense (const CDense & other) : CMatrix (other.m_m, other.m_n) {
    m_matrix.resize(m_m);
    m_matrix.reserve(m_m);
    for (int i = 0; i < m_m; ++i) {
        for (int j = 0; j < m_n; j++)  
        {
            m_matrix[i].push_back(other.m_matrix[i][j]);
        }
    }
}
CDense & CDense::operator = (const CDense & other) {
    if (&other == this) return *this;
    CDense temp (other);
    std::swap (temp.m_m, m_m);
    std::swap (temp.m_n, m_n);
    std::swap (temp.m_matrix, m_matrix);
    return *this;
}

bool CDense::NotSet          (void) const {
    return m_n == 0;
}

DensePtr operator + (const CDense & left, const CDense & right) {
    if (! (left.m_n == right.m_n && left.m_m == right.m_m)) throw WrongFormat();
    std::shared_ptr<CDense> temp = std::make_shared<CDense>(right.m_m, right.m_n);
    for (int i = 0; i < temp->m_m; i++) {
        temp->m_matrix[i].resize(temp->m_n);
        temp->m_matrix[i].reserve(temp->m_n);
    }
    for (int i = 0; i < left.m_m; ++i) {
        for (int j = 0; j < left.m_n; j++)  
        {
            temp ->m_matrix[i][j] =
            left  .m_matrix[i][j] +
            right .m_matrix[i][j]; 
        }
    } 
    return temp;
}

std::shared_ptr<CDense> CDense::operator - (const CDense & other) const {
    if (! (m_n == other.m_n && m_m == other.m_m)) throw WrongFormat();
    std::shared_ptr<CDense> temp = std::make_shared<CDense> (other.m_m, other.m_n);
    for (int i = 0; i < temp->m_m; i++) {
        temp->m_matrix[i].resize(temp->m_n);
        temp->m_matrix[i].reserve(temp->m_n);
    }
    for (int i = 0; i < m_m; ++i) {
        for (int j = 0; j < m_n; j++)  
        {
            temp->m_matrix[i][j] = m_matrix[i][j] - other.m_matrix[i][j]; 
        }
    } 

    return temp;
}

// shared_ptr<CDense> operator * (float alpha, const CDense & matrix){
//     shared_ptr<CDense> temp = make_shared<CDense> (matrix.m_m, matrix.m_n);
//     for (int i = 0; i < temp.m_m; i++) {
//         temp.m_matrix[i].resize(temp.m_n);
//         temp.m_matrix[i].reserve(temp.m_n);
//     }
//     for (int i = 0; i < matrix.m_m; ++i) {
//         for (int j = 0; j < matrix.m_n; j++)  
//         {
//             temp.m_matrix[i][j] = matrix.m_matrix[i][j] * alpha;
//         }
//     } 

//     return temp;
// }

// shared_ptr<CDense> operator * (const CDense & left, const CDense & right) {
//     if (left.m_n != right.m_m) throw WrongFormat ();
//     shared_ptr<CDense> temp = make_shared <CDense> (left.m_m, right.m_n);
    

//     return temp;
// }


void CDense::Print(std::ostream & out) const {
    for (size_t i = 0; i < m_matrix.size(); ++i) {
        out << '(';
        for (size_t j = 0; j < m_matrix[i].size(); j++)  
            out << m_matrix[i][j] << (j != m_matrix[i].size() - 1 ? " " : "");
        out << ')';
        if (i != m_matrix.size() - 1) out << std::endl;
    } 
}

void CDense::Read (std::istream & in) {
    std::string str;
    getline (in, str, ']');
    // str += ']';
    std::stringstream is;
    is << str;
    if (m_n == 0) {
        FindSize (str);
    }
    
    char character;
    double temp;
    is >> character; if (character != '[') throw WrongFormat();
    for (long i = 0; i < m_m; ++i) {
        for (long j = 0; j < m_n; j++)  
        {
            if (!( is >> temp)) throw WrongDimensions();
            m_matrix[i].push_back(temp);  
        }
        if (i != m_m - 1 && ( (! (is >> character) || character != ';')))
            throw WrongFormat();
    } 
}

// std::istream & operator >> (std::istream & in, CDense & matrix) {
//     std::string str;
//     getline (in, str, ']');
//     // str += ']';
//     std::stringstream is;
//     is << str;
//     if (matrix.m_n == 0) {
//         matrix.FindSize (str);
//     }
    
//     char character;
//     double temp;
//     is >> character; if (character != '[') throw WrongFormat();
//     for (long i = 0; i < matrix.m_m; ++i) {
//         for (long j = 0; j < matrix.m_n; j++)  
//         {
//             if (!( is >> temp)) throw WrongDimensions();
//             matrix.m_matrix[i].push_back(temp);  
//         }
//         if (i != matrix.m_m - 1 && ( (! (is >> character) || character != ';')))
//             throw WrongFormat();
//     } 
//     // is >> character; if (character != ']') throw WrongDimensions();
//     return in;
// }

void CDense::FindSize (std::string & src) {
    std::istringstream is (src);
    int cnt_n = 0;
    int cnt_m = 1;
    float temp;
    char c;
    is >> c;
    //finding how many columns matrix has: 
    do {
        if ( ! (is >> temp) ) break;
        cnt_n++;
    } while ( !is.eof() );
    //finding how many rows it has:
    long unsigned int i = 0;
    do {
        if (src[i] == ';')
            cnt_m ++; 
        i++;
    } while (i != src.size() - 1);
    std::cout << "matrix has " << cnt_m << " rows and " << cnt_n << " columns\n";

    m_n = cnt_n;
    m_m = cnt_m;
    m_matrix.resize(m_m);
    m_matrix.reserve(m_m);
}

CMatrix * CDense::Clone () {
    // CDense temp (*this);
    return new CDense (*this);
}


CDense::~CDense () {}