#ifndef TMVA_SOFIE_ROPERATOR_TOPK
#define TMVA_SOFIE_ROPERATOR_TOPK

#include "TMVA/SOFIE_common.hxx"
#include "TMVA/ROperator.hxx"
#include "TMVA/RModel.hxx"

#include <sstream>

namespace TMVA {
namespace Experimental {
namespace SOFIE {

template <typename T>
class ROperator_TopK final : public ROperator {

private:
   int fAttrAxis;
   int fAttrLargest;
   int fAttrSorted;

   std::vector<int> k;
   std::string fNX;
   std::string fNY;
   std::vector<size_t> fShapeX;
   std::vector<size_t> fShapeY;
   std::string fType;

public:
   ROperator_TopK() {}
   ROperator_TopK(int attr_axis, int attr_largest, int attr_sorted, std::vector<int>kElem, std::string nameX, std::string nameY)
      : fAttrAxis(attr_axis),
        fAttrLargest(attr_largest),
        fAttrSorted(attr_sorted),
        k(kElem),
        fNX(UTILITY::Clean_name(nameX)),
        fNY(UTILITY::Clean_name(nameY))
   {
   }

   std::vector<ETensorType> TypeInference(std::vector<ETensorType> input) { return input; }

   std::vector<std::vector<size_t>> ShapeInference(std::vector<std::vector<size_t>> input)
   {
      auto ret = input; // notsure *******************
      return ret;
   }

   void Initialize(RModel &model)
   {
      if (model.CheckIfTensorAlreadyExist(fNX) == false) {
         // input must be a graph input, or already initialized intermediate tensor
         throw std::runtime_error("TMVA SOFIE TopK Op Input Tensor is not found in model");
      }
      fShapeX = model.GetTensorShape(fNX); //  [ m x n x o x p ... ]
      if(k[0]>=fShapeX.size()){
         throw
            std::runtime_error("TMVA::SOFIE ONNX TopK op k = "+ std::to_string(k[0]) +"value exeeds size of tensor " +fNX+" of size "+fShapeX.size()+" .");
      }
      fShapeY.push_back(2);
      for (auto i : fShapeX)
         fShapeY.push_back(i); //  [ 2 x m x n x o x p ... ]
      size_t axis = fAttrAxis < 0 ? fShapeX.size() + fAttrAxis : fAttrAxis;
      fShapeY[axis] = k[0]; //  [ 2 x m x n x K x p ... ]
      model.AddIntermediateTensor(fNY, model.GetTensorType(fNX), fShapeY);
      fType = ConvertTypeToString(model.GetTensorType(fNX));
   }

   std::string Generate(std::string OpName)
   {
      OpName = "op_" + OpName;
      if (fShapeX.empty()) {
         throw std::runtime_error("TMVA SOFIE Operator TopK called to Generate without being initialized first");
      }
      std::stringstream out;
      size_t size = fShapeX.size();
      size_t axis = fAttrAxis < 0 ? size + fAttrAxis : fAttrAxis;
      out << "\n" << SP << "//------ TopK\n";

      // out << "long long int size = " << size << ";\nvector<int>fShape = " << fShapeX << ";\n";
      // out << "long long int axis=" << axis << ";\nint k= " << k << ";\nstring fType = \"" << fType << "\";\n";
      // out << "bool sorted = " << fAttrSorted << ";\nbool max  = " << fAttrLargest << ";";

      std::vector<std::string> id;
      for (size_t i = 1; i < size; ++i) {
         std::string ss;
         ss = "i_" + std::to_string(i);
         id.push_back(ss);
      }

      int dim = 0;
      for (size_t a = 0; a < size; a++) {
         if (a == size - 1) {
            out << "std::vector<pair<" << fType << "," << fType << ">>elements;\n";
            for (size_t i = 0; i < a; i++)
               out << SP;
         }
         out << "for(int " << id[a] << "=0;" << id[a] << " < "
             << fShapeX[((a < size - 1) ? ((a == axis) ? ++dim : dim) : axis)] << id[a] << "++){\n";
         for (size_t i = 0; i <= a; i++)
            out << SP;
         dim++;
      }
      std::string element = "tensor_" + fNX;
      std::string indexes = "";
      for (size_t i = 0; i < size; i++) {
         indexes += "[";
         if (i == axis) {
            indexes += id[size - 1];
            dim--;
         } else
            indexes += id[dim];
         indexes += "]";
         dim++;
      }
      element += indexes;
      out << "elements.push_back({" << element << "," << id[size - 1] << "});\n";
      for (size_t i = 0; i < size - 1; i++)
         out << SP;
      out << "}\n";
      for (size_t i = 0; i < size - 1; i++)
         out << SP;
      if (fAttrSorted) {
         if (fAttrLargest) {
            out << "std::sort(elements.begin(),elements.end(),[](pair<int,int>a,pair<int,int>b){return "
                   "a.first>b.first;})";
         } else
            out << "std::sort(elements.begin(),elements.end(),[](pair<int,int>a,pair<int,int>b){return "
                   "a.first<b.first;})";
      } else
         out << "std::sort(elements.begin(),elements.end())";
      out << ";\n";
      for (size_t i = 0; i < size - 1; i++)
         out << SP;
      std::string itr = (id[size - 1]);
      out << "for(int " << itr << "=0;" << itr << "<" << std::to_string(k[0]) << ";" << itr << "++) {\n";
      for (size_t i = 0; i < size; i++)
         out << SP;
      out << "tensor_" << fNY << "[0]" << indexes << " = elements[" << itr << "].first;\n";
      for (size_t i = 0; i < size; i++)
         out << SP;
      out << "tensor_" << fNY << "[1]" << indexes << "=elements[" << itr << "].second;\n";
      for (size_t i = 0; i < size - 1; i++)
         out << SP;
      out << "}\n";
      for (size_t c = size - 1; c >= 0; c--) {
         for (size_t i = c - 2; i >=0; i--)
            out << SP;
         out << "}\n";
      }
      return out.str();
   }
};

} // nameSPace SOFIE
} // nameSPace Experimental
} // nameSPace TMVA

#endif // TMVA_SOFIE_ROPERATOR_TOPK