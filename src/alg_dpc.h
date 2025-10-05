#ifndef ALG_DPC_H
#define ALG_DPC_H

#include <vector>
#include <cstdint>

// 算法模型使用的数据类型
using alg_pixel_t = uint16_t;

template <typename ALG_INPUT_DATA_TYPE, typename ALG_OUTPUT_DATA_TYPE>
class AlgDpc {
public:
    
    static std::vector<alg_pixel_t> process_image(
        const std::vector<alg_pixel_t>& input_image,
        int width, int height,
        bool enable,
        int threshold
    );
};

#endif // ALG_DPC_H
