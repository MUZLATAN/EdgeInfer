#include <thread>
#include <chrono>
#include <iostream>
#include "infer_engine_api.h"
#include "ie_params.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"

#include "VideoInput.hpp"


#define PRO_ROOT "/home/embed/AIHubInfer/" 

int main(int argc, char* argv[])
{
     //获取图片
    VideoInput video_input;
    video_input.open("rtsp://admin:whale123@192.168.10.48:554/h264/ch39/main/av_stream");

    //初始化engine
    std::string model_path=std::string(PRO_ROOT)+"models/cardet_800_480_1_0_5.rknn";
    //图片预处理参数
    IE_PreParam preparam(
        IE_PREPROCESS_RGB_ORDER::BGR,
        IE_PREPROCESS_FORMAT::NCHW,
        std::vector<float>{0.5f, 0.5f, 0.5f},
        std::vector<float>{1.0f, 1.0f, 1.0f}
    );
    IE_PostParam postparam;
    IE_Param param(preparam,postparam);
    int ret = 0;

    //创建推理引擎
    IE_Engine* engine = create_engine(model_path.c_str(),param, &ret);

    //推理
    std::vector<IE_Blob> outputs;
    float time_cnt = 0;

    int read_cnt = 0;
    cv::Mat img;
    while(true)
    {
        auto start = std::chrono::steady_clock::now();
                
        if (!video_input.read(img) && read_cnt % 20 == 0){
            std::cout<<"[ "<<__TIMESTAMP__<< " main.cpp"<<__LINE__<<" ]"<<" read failed"<<std::endl;
        }
        run_engine(engine, img, outputs);   
        
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::milli> du = end - start;
        time_cnt += du.count();
        if (read_cnt % 20 == 0)
            std::cout<<"[ "<<__TIMESTAMP__<< " main.cpp"<<__LINE__<<" ]"<<"Inference Cost  "<<"  "<<du.count()<<" ms"<<std::endl;

        read_cnt++;
    }
    std::cout<<"time arevage: ---- "<<time_cnt /read_cnt<<std::endl;

    //销毁资源
    destroy_engine(engine);
    

return 0;



    int num_threads = 5;
    int loop_cnt = 100;
    for (int idx = 0; idx < num_threads; idx++)
    {
        std::thread tt([&](){

            //创建推理引擎
            IE_Engine* engine = create_engine(model_path.c_str(),param, &ret);

            //推理
            std::vector<IE_Blob> outputs;
            float time_cnt = 0;
            for (int loop = 0; loop < loop_cnt; loop ++)
            {
                auto start = std::chrono::steady_clock::now();
                
                run_engine(engine, img, outputs);   
                
                auto end = std::chrono::steady_clock::now();
                std::chrono::duration<float, std::milli> du = end - start;
                time_cnt += du.count();
                printf("[Thread: %u]  Inference Cost %.2f ms\n", std::this_thread::get_id(), du.count());
            }

            printf("time cost average: %f \n",  time_cnt/loop_cnt);
            

            //销毁资源
            destroy_engine(engine);

        });
        tt.detach();
    }




    std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());

    return 0;
}