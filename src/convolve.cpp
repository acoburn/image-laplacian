#include <iostream>
#include <string>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <laplacian/version.hpp>
#include <laplacian/convolve.hpp>
#include <laplacian/kernel/gaussian.hpp>
#include <laplacian/kernel/laplace.hpp>
#include <laplacian/laplacian.hpp>

#define usage() \
    std::cerr << "Usage: " << argv[0] << " [options] input output" << std::endl << std::endl; \
    std::cerr << "Options: " << std::endl; \
    std::cerr << opts << std::endl; \


namespace po = boost::program_options;

template<typename T>
int convolve(const std::string& input, const std::string& output) {
    try {
        laplacian::Convolution<T> smoother;
        smoother.convolve(input, output);
        return EXIT_SUCCESS;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

bool valid_kernel(const int size) {
    return size >= 1 && size <= 4;
}

int main(int argc, char* argv[]) {

    po::variables_map vm;

    po::options_description opts;
    opts.add_options()
        ("help", "produce this help message")
        ("version", "print version information")
        ("gaussian,g", po::value<int>(), "gaussian sigma")
        ("laplacian,l", po::value<int>(), "laplacian scale")
        ("input,i",  po::value<std::string>(), "input file")
        ("output,o", po::value<std::string>(), "output file")
        ;

    po::positional_options_description positional_opts;
    positional_opts.add("input", 1);
    positional_opts.add("output",  2);
    po::store(po::command_line_parser(argc, argv).options(opts).positional(positional_opts).run(), vm);

    try {
        po::notify(vm);
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        usage();
        return EXIT_FAILURE;
    }

    if (vm.count("help")){
        usage();
        return EXIT_SUCCESS;
    }

    if (vm.count("version")) {
        std::cerr << std::endl;
        laplacian::print_version_info(std::cerr);
        std::cerr << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("input")) {

        const std::string input(vm["input"].as<std::string>());
        std::string output;
        if (vm.count("output")) {
            output = vm["output"].as<std::string>();
        } else {
            output = vm["input"].as<std::string>() + ".out";
            std::cerr << "Using " << output << " as the output file." << std::endl;
        }

        if (vm.count("laplacian") && valid_kernel(vm["laplacian"].as<int>())) {
            std::cerr << "Using laplacian (scale=" << vm["laplacian"].as<int>() << ") convolution" << std::endl;
            switch(vm["laplacian"].as<int>()) {
                case 1:
                    return convolve<laplacian::kernel::laplace::Scale1>(input, output);
                case 2:
                    return convolve<laplacian::kernel::laplace::Scale2>(input, output);
                case 3:
                    return convolve<laplacian::kernel::laplace::Scale3>(input, output);
                case 4:
                    return convolve<laplacian::kernel::laplace::Scale4>(input, output);
            }
        } else if (vm.count("gaussian") && valid_kernel(vm["gaussian"].as<int>())) {
            std::cerr << "Using gaussian (sigma=" << vm["gaussian"].as<int>() << ") convolution" << std::endl;
            switch(vm["gaussian"].as<int>()) {
                case 1:
                    return convolve<laplacian::kernel::gaussian::Sigma1>(input, output);
                case 2:
                    return convolve<laplacian::kernel::gaussian::Sigma2>(input, output);
                case 3:
                    return convolve<laplacian::kernel::gaussian::Sigma3>(input, output);
                case 4:
                    return convolve<laplacian::kernel::gaussian::Sigma4>(input, output);
            }
        } else {
            std::cerr << "Invalid algorithm specified!" << std::endl;
            usage();
            return EXIT_FAILURE;
        }
    } else {
        std::cerr << "Missing input file" << std::endl;
        usage();
        return EXIT_FAILURE;
    }
}


