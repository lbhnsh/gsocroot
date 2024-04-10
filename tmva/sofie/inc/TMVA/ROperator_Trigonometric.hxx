#ifndef TMVA_SOFIE_ROPERATOR_Trigonometric
#define TMVA_SOFIE_ROPERATOR_Trigonometric

#include "TMVA/SOFIE_common.hxx"
#include "TMVA/ROperator.hxx"
#include "TMVA/RModel.hxx"

#include <sstream>

namespace TMVA{
namespace Experimental{
namespace SOFIE{

enum ETrigonometricOperator { Sin, Sinh, Asin, Asinh, Cos, Cosh, Acos, Acosh, Tan, Tanh, Atan, Atanh };

template <typename T, EBasicTrigonometricOperator Op1>
struct TrigonometricOperatorTrait {};

template <typename T>
struct TrigonometricOperatorTrait<T, Sin> {
  static const std::string Name() { return "Sin"; }
  static std::string Op(const std::string t) { return "std::sin(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Sinh> {
  static const std::string Name() { return "Sinh"; }
  static std::string Op(const std::string t) { return "std::sinh(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Asin> {
  static const std::string Name() { return "Asin"; }
  static std::string Op(const std::string t) { return "std::asin(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Asinh> {
  static const std::string Name() { return "Asinh"; }
  static std::string Op(const std::string t) { return "std::asinh(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Cos> {
  static const std::string Name() { return "Cos"; }
  static std::string Op(const std::string t) { return "std::cos(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Cosh> {
  static const std::string Name() { return "Cosh"; }
  static std::string Op(const std::string t) { return "std::cosh(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Acos> {
  static const std::string Name() { return "Acos"; }
  static std::string Op(const std::string t) { return "std::acos(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Acosh> {
  static const std::string Name() { return "Acosh"; }
  static std::string Op(const std::string t) { return "std::acosh(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Tan> {
  static const std::string Name() { return "Tan"; }
  static std::string Op(const std::string t) { return "std::tan(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Tanh> {
  static const std::string Name() { return "Tanh"; }
  static std::string Op(const std::string t) { return "std::tanh(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Atan> {
  static const std::string Name() { return "Atan"; }
  static std::string Op(const std::string t) { return "std::atan(" + t + ")"; }
};

template <typename T>
struct TrigonometricOperatorTrait<T, Atanh> {
  static const std::string Name() { return "Atanh"; }
  static std::string Op(const std::string t) { return "std::atanh(" + t + ")"; }
};

template<typename T, ETrigonometricOperator Op>
class ROperator_Trigonometric final : public ROperator{
private:

   std::string fNX;
   std::string fNY;
   std::vector<size_t> fShape;

public:
   ROperator_Tanh(){}
   ROperator_Tanh(std::string nameX, std::string nameY):
      fNX(UTILITY::Clean_name(nameX)), fNY(UTILITY::Clean_name(nameY)){}

   std::vector<ETensorType> TypeInference(std::vector<ETensorType> input){
      return input;
   }

   std::vector<std::vector<size_t>> ShapeInference(std::vector<std::vector<size_t>> input){
      auto ret = input; //suggest copy to compiler
      return ret;
   }

   void Initialize(RModel& model){
       //input must be a graph input, or already initialized intediate tensor
      if (model.CheckIfTensorAlreadyExist(fNX) == false){
        throw std::runtime_error(std::string("TMVA SOFIE Trigonometric Op Input Tensor ") + fNX + " is not found in model");
      }
      fShape = model.GetTensorShape(fNX);
      model.AddIntediateTensor(fNY, model.GetTensorType(fNX), fShape);

   }

   std::string Generate(std::string OpName){
      OpName = "op_" + OpName;
      if (fShape.empty()) {
         throw std::runtime_error("TMVA SOFIE Trigonometric operator called to Generate without being initialized first");
      }
      std::stringstream out;
      size_t length = ConvertShapeToLength(fShape);
      out << "\n//------ "<<TrigonometricOperatorTrait<T,Op>::Name()<<"\n";
      out << SP << "for (int id = 0; id < " << length << " ; id++){\n";
      out << SP << SP << "tensor_" << fNY << "[id] = "<< TrigonometricOperatorTrait<T,Op>::Op("tensor_" + fNX + "[id]")<<";\n";
      out << SP << "}\n";
      return out.str();
   }

   std::vector<std::string> GetStdLibs() { return { std::string("cmath") };}
};

}//SOFIE
}//Experimental
}//TMVA


#endif //TMVA_SOFIE_ROPERATOR_Tanh
