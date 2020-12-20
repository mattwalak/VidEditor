#ifndef FUNMATH_H
#define FUNMATH_H


VEC3 extend(VEC2 v, float f){
    return VEC3(v[0], v[1], f);
}

float triArea(const VEC3& a, const VEC3& b){
    VEC3 cross = a.cross(b);
    int sign = (cross[2] < 0 ? -1 : 1);
    return cross.norm()*sign/2;
}

// Returns the first real root of a cubic between 0 and 1
// CREDIT: http://www.cplusplus.com/forum/beginner/234717/
float solveCubic(float a, float b, float c, float d) 
{    
    b /= a;
    c /= a;
    d /= a;
    
    int case_num = -1;

    double disc, q, r, dum1, s, t, term1, r13;
    q = (3.0*c - (b*b))/9.0;
    r = -(27.0*d) + b*(9.0*c - 2.0*(b*b));
    r /= 54.0;
    disc = q*q*q + r*r;
    term1 = (b/3.0);
    
    double x1_real = 0, x2_real = 0, x3_real = 0;
    double x2_imag = 0, x3_imag = 0;
    std::string x2_imag_s, x3_imag_s;
    if (disc > 0)   // One root real, two are complex
    {
        case_num = 0;
        s = r + sqrt(disc);
        s = s<0 ? -cbrt(-s) : cbrt(s);
        t = r - sqrt(disc);
        t = t<0 ? -cbrt(-t) : cbrt(t);
        x1_real = -term1 + s + t;
        term1 += (s + t)/2.0;
        x3_real = x2_real = -term1;
        term1 = sqrt(3.0)*(-t + s)/2;
        x2_imag = term1;
        x3_imag = -term1;
        x2_imag_s =  " + "+ std::to_string(x2_imag) + "i";
        x3_imag_s =  " - "+ std::to_string(x2_imag) + "i";
    } 
    // The remaining options are all real
    else if (disc == 0)  // All roots real, at least two are equal.
    { 
        case_num = 1;
        x3_imag = x2_imag = 0;
        r13 = r<0 ? -cbrt(-r) : cbrt(r);
        x1_real = -term1 + 2.0*r13;
        x3_real = x2_real = -(r13 + term1);
    }
    // Only option left is that all roots are real and unequal (to get here, q < 0)
    else
    {
        case_num = 2;
        x3_imag = x2_imag = 0;
        q = -q;
        dum1 = q*q*q;
        dum1 = acos(r/sqrt(dum1));
        r13 = 2.0*sqrt(q);
        x1_real = -term1 + r13*cos(dum1/3.0);
        x2_real = -term1 + r13*cos((dum1 + 2.0*M_PI)/3.0);
        x3_real = -term1 + r13*cos((dum1 + 4.0*M_PI)/3.0);
    }
    
    /*std::cout << "\nRoots:" << std::endl <<
            "  x = " << x1_real << std::endl <<
            "  x = " << x2_real << x2_imag_s << std::endl <<
            "  x = " << x3_real << x3_imag_s << std::endl;*/
    
    if(x2_imag == 0){
        // All three are real
        if(x1_real >= 0 && x1_real <= 1){
            return x1_real;
        }else if(x2_real >= 0 && x2_real <= 1){
            return x2_real;
        }else if(x3_real >= 0 && x3_real <= 1){
            return x3_real;
        }else{
            ERROR("ERROR - FUNMATH - Cubic has no real solution between 0 and 1");
            std::cout << "\nRoots:" << std::endl <<
            "  x = " << x1_real << std::endl <<
            "  x = " << x2_real << x2_imag_s << std::endl <<
            "  x = " << x3_real << x3_imag_s << std::endl;
            PRINT("case_num = " << case_num);
            return -1;
        }
    }else{
        if(x1_real >= 0 && x1_real <= 1){
            return x1_real;
        }else{
            ERROR("ERROR - FUNMATH - Cubic has no real solution between 0 and 1");
            std::cout << "\nRoots:" << std::endl <<
            "  x = " << x1_real << std::endl <<
            "  x = " << x2_real << x2_imag_s << std::endl <<
            "  x = " << x3_real << x3_imag_s << std::endl;
            PRINT("case_num = " << case_num);
            return -1;
        }
    }


    return 0; 
}

#endif // FUNMATH_H