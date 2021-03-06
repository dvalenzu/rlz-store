#define ELPP_THREAD_SAFE

#include "utils.hpp"
#include "collection.hpp"
#include "rlz_utils.hpp"

#include "indexes.hpp"

#include "logging.hpp"
INITIALIZE_EASYLOGGINGPP

int main(int argc, const char* argv[])
{
    setup_logger(argc, argv);

    /* parse command line */
    LOG(INFO) << "Parsing command line arguments";
    auto args = utils::parse_args(argc, argv);

    /* parse the collection */
    LOG(INFO) << "Parsing collection directory " << args.collection_dir;
    collection col(args.collection_dir);

    /* create rlz index */
    const uint32_t factorization_blocksize = 64 * 1024;
    {
        auto lz_store = typename lz_store_static<coder::brotlih<6>, factorization_blocksize>::builder{}
                            .set_rebuild(args.rebuild)
                            .set_threads(args.threads)
                            .build_or_load(col);
        verify_index(col, lz_store);
    }
    {
        auto lz_store = typename lz_store_static<coder::zlib<9>, factorization_blocksize>::builder{}
                            .set_rebuild(args.rebuild)
                            .set_threads(args.threads)
                            .build_or_load(col);
        verify_index(col, lz_store);
    }

    return EXIT_SUCCESS;
}
