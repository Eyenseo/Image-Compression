#include <iostream>
#include <exception>

#include "PPMFile.hpp"
#include "DiffPic.hpp"
#include "DCTFile.hpp"
#include "FMatrix.hpp"

#include "Huffile.hpp"

const std::string full_help = 
  "\nThe program can encode any file via the Huffman entropy encoding.\n"
  "Additional it can transform a ppm file with the discrete cosiuns\n"
  "transformation.\n"
  "It is able to Work with P1, P2, P3, P4, P5 and P6.\n"
  "Also it can display the difference between two pictures that went through\n"
  "the DCT\n"  
  "\nUsage:\n"
  "  main -h <input> <output> (--en|--de) [--detail]\n"
  "    Example:\n"
  "      main -h ./myFile.whatEver ./compressed.humdidum --en\n\n"
  "  main -d <input> <output> (--en|--de) [--detail] [--quality] [--quanti]\n"
  "    Example:\n"
  "      main -d ./myPicture.ppm ./compressed.humdi --en --quality 50\n\n"
  "  main -p <first> <second> <output> [--rmse]\n"
  "    Example:\n"
  "      main -p ./myFirstPicture.ppm ./mySecondPicture.ppm ./difference.ppm "
  "--rmse\n\n"
  "\nArguments:\n"
  "  -h:    Selects the Huffman encoding / decoding\n"
  "    <input>             Specifies the input file\n"
  "    <output>            Specifies the output file\n"
  "    --en                Sets the mode to encoding\n"
  "    --de                Sets the mode to decoding\n"
  "    --detail            Will show details of the compression\n"
  "\n  -d:    Selects the Discrete Cosinus Transformation for ppm files\n"
  "    <input>             Specifies the input file\n"
  "    <output>            Specifies the output file\n"
  "    --en                Sets the mode to encoding\n"
  "    --de                Sets the mode to decoding\n"
  "    --detail            Will show details of the compression\n"
  "    --quality <number>  Sets the quality of the compression in range of 0\n"
  "                        and 100. (Default 100)\n"
  "    --quanti <file>     Loads a quantisation matrix from a file.\n"
  "                        The format of the Quantisation Matrix has to be\n"
  "                        the following:\n"
  "                        It has to be an 8x8 matrix. There are only values\n"
  "                        allowed between 0 and 255. Each value has to be\n"
  "                        seperated by a space or a tab. The value may only\n" 
  "                        be Integers.\n"
  "                        Example:\n"
  "                          16 11 10 16 24 40 51 61\n"
  "                          12 12 14 19 26 58 60 55\n"
  "                          14 13 16 24 40 57 69 56\n"
  "                          14 17 22 29 51 87 80 62\n"
  "                          18 22 37 56 68 109 103 77\n"
  "                          24 35 55 64 81 104 113 92\n"
  "                          49 64 78 87 103 121 120 101\n"
  "                          72 92 95 98 112 100 103 99\n"
  "\n  -p:    Selects the difference picture\n"
  "    <first>             Specifies the first Image (original)\n"
  "    <second>            Specifies the second Image (transformed)\n"
  "    <output>            Specifies the output file\n"
  "    --rmse:             Will print the root mean square error of the two \n"
  "                        pictures\n"
  "\n  --help:             Show this help\n";

/**
 * the method interprets the arguments for the Huffmann encoding of a file
 * 
 * @param  argc amount of given parameters
 * @param  argv array with parameter as c-str
 * @return      returns 0 if all parameters are interpreted correctly and 1 of a
 *              parameter is not interpretable
 */
int huffile(const int &argc, char** argv) {
  std::string input = std::string(argv[2]);
  std::string output = std::string(argv[3]);
  bool encoding = true;

  if( argv[4][0] == '-'
      && argv[4][1] == '-'
      && argv[4][2] == 'd'
      && argv[4][3] == 'e') {
    encoding = false;
  }

  Huffile *h = new Huffile(input, encoding);
  
  if(encoding) {
    h->write_encoded_to(output);
  } else {
    h->write_decoded_to(output);
  }

  if( argc == 6 
      && argv[5][0] == '-'
      && argv[5][1] == '-'
      && argv[5][2] == 'd'
      && argv[5][3] == 'e'
      && argv[5][4] == 't'
      && argv[5][5] == 'a'
      && argv[5][6] == 'i'
      && argv[5][7] == 'l') {
    std::cout << "Entropy: " << h->get_entropy_rate() << "\nRedundancy: " 
    << h->get_redundancy() << "\nCompressed in Byte: "
    << h->get_encoded_data_size() << "\nCompression rate: " 
    << h->get_decoded_data_size() / h->get_encoded_data_size() << std::endl;
  }

  delete h;

  return 0;
}

/**
 * the method interprets the arguments for the Discrete Cosinus Transformation
 * encoding of a file
 * 
 * @param  argc amount of given parameters
 * @param  argv array with parameter as c-str
 * @return      returns 0 if all parameters are interpreted correctly and 1 of a
 *              parameter is not interpretable
 */
int dct(const int &argc, char** argv) {
  std::string input = std::string(argv[2]);
  std::string output = std::string(argv[3]);
  bool show_detail = false;
  bool encoding = true;
  unsigned char quality = 100;
  Matrix<unsigned char> *quanti = NULL;

  if( argv[4][0] == '-'
      && argv[4][1] == '-'
      && argv[4][2] == 'd'
      && argv[4][3] == 'e') {
    encoding = false;
  }

  for(int i = 5; i < argc; ++i) {
    if( argv[i][0] == '-'
        && argv[i][1] == '-'
        && argv[i][2] == 'q'
        && argv[i][3] == 'u'
        && argv[i][4] == 'a'
        && argv[i][5] == 'l'
        && argv[i][6] == 'i'
        && argv[i][7] == 't'
        && argv[i][8] == 'y') {
      if(i + 1 < argc) {
        quality = atoi(argv[++i]);
      } else {
        std::cout << full_help;
        return 1;
      }

      if(!encoding) {
        std::cout << "Quality settings only apply to encoding." << std::endl;
      }
    } else if(argv[i][0] == '-'
              && argv[i][1] == '-'
              && argv[i][2] == 'q'
              && argv[i][3] == 'u'
              && argv[i][4] == 'a'
              && argv[i][5] == 'n'
              && argv[i][6] == 't'
              && argv[i][8] == 'i') {
      if(i + 1 < argc) {
        quanti = new FMatrix<unsigned char>(argv[++i]);
      } else {
        std::cout << full_help;
        return 1;
      }

      if(!encoding) {
        std::cout << "Quantisation settings only apply to encoding."
        << std::endl;
      }
    } else if( argv[i][0] == '-'
        && argv[i][1] == '-'
        && argv[i][2] == 'd'
        && argv[i][3] == 'e'
        && argv[i][4] == 't'
        && argv[i][5] == 'a'
        && argv[i][6] == 'i'
        && argv[i][7] == 'l') {
      show_detail = true;
    } else {
      std::cout << full_help;
      return 1;
    }
  }
  
  if(encoding) {
    PPMFile<double> *p = new PPMFile<double>(input); 
    DCTFile *d = new DCTFile(p->get_mat_pic(), p->get_color_depth()); 

    if(quanti != NULL) {
      d->set_quantization(*quanti);
    }
    d->set_quality(quality); 
    d->forward_dct();
    d->write_to(output);

    if(show_detail) {
      std::cout << "Compression rate: "
      << p->get_file_size() / d->get_file_size() << std::endl;
    }

    delete p;
    delete d;
  } else {
    PPMFile<double> *p = new PPMFile<double>();
    DCTFile *d = new DCTFile(input);
    
    d->inverse_dct();
    p->set_mat_pic(d->get_mat_pic(), d->get_color_depth());
    p->write_to(output);
    
    if(show_detail) {
      std::cout << "Compression rate: "
      << p->get_file_size() / d->get_file_size() << std::endl;
    }

    delete p;
    delete d;
  }
  delete quanti;
  return 0;
}

/**
 * the method interprets the arguments for the difference Picture encoding of a
 * file
 * 
 * @param  argc amount of given parameters
 * @param  argv array with parameter as c-str
 * @return      returns 0 if all parameters are interpreted correctly and 1 of a
 *              parameter is not interpretable
 */
int diff_pic(const int &argc, char** argv) {
  std::string first = std::string(argv[2]);
  std::string second = std::string(argv[3]);
  std::string output = std::string(argv[4]);

  PPMFile<double> *first_pic = new PPMFile<double>(first);
  PPMFile<double> *second_pic = new PPMFile<double>(second);

  DiffPic *diff_pic = new DiffPic();
  diff_pic->set_pic_one(first_pic->get_mat_pic());
  diff_pic->set_pic_two(second_pic->get_mat_pic());
  diff_pic->calculate();
  diff_pic->write_to(output);

  if( argc == 6
      && argv[5][0] == '-'
      && argv[5][1] == '-'
      && argv[5][2] == 'r'
      && argv[5][3] == 'm'
      && argv[5][4] == 's'
      && argv[5][5] == 'e') {
    std::cout << "RMSE: " << diff_pic->get_rmse() << std::endl;
  }

  delete first_pic;
  delete second_pic;
  delete diff_pic;
  return 0;
}

int main(int argc, char** argv) {
  try {
    if(argc < 2) {
      std::cout << full_help;
      return 1;
    } else if(argc > 4) {
      if(argv[1][0] == '-' && argv[1][1] == 'h') {
        return huffile(argc, argv);
      } else if(argv[1][0] == '-' && argv[1][1] == 'd') {
        return dct(argc, argv);
      } else if(argv[1][0] == '-' && argv[1][1] == 'p') {
        return diff_pic(argc, argv);
      }
    } else {
      std::cout << full_help;
      return 1;
    }
  } catch(const std::exception &e) {
    std::cout << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  } catch(...) {
    std::cout << std::endl;
    std::cerr << "Unknown Exception found ..." << std::endl;
    return 1;
  }
  return 0;
}
