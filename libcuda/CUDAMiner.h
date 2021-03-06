/*  Blah, blah, blah.. all this pedantic nonsense to say that this
    source code is made available under the terms and conditions
    of the accompanying GNU General Public License */

#pragma once

#include <time.h>
#include <functional>
#include <libethash/ethash.h>
#include <libdevcore/Worker.h>
#include <libethcore/EthashAux.h>
#include <libethcore/Miner.h>
#include "ethash_cuda_miner_kernel.h"
#include "libethash/internal.h"

namespace dev
{
namespace eth
{

class CUDAMiner: public Miner
{

public:
	CUDAMiner(FarmFace& _farm, unsigned _index);
	~CUDAMiner() override;

	static unsigned instances()
	{
		return s_numInstances > 0 ? s_numInstances : 1;
	}
	static unsigned getNumDevices();
	static void listDevices();
	static void setParallelHash(unsigned _parallelHash);
	static bool configureGPU(
	    unsigned _blockSize,
	    unsigned _gridSize,
	    unsigned _numStreams,
	    unsigned _scheduleFlag,
	    uint64_t _currentBlock,
	    unsigned _dagLoadMode,
	    unsigned _dagCreateDevice,
	    bool _eval
	);
	static void setNumInstances(unsigned _instances);
	static void setDevices(const vector<unsigned>& _devices, unsigned _selectedDeviceCount);
	static bool cuda_configureGPU(
	    size_t numDevices,
	    const vector<int>& _devices,
	    unsigned _blockSize,
	    unsigned _gridSize,
	    unsigned _numStreams,
	    unsigned _scheduleFlag,
	    uint64_t _currentBlock,
	    bool _eval
	);

	static void cuda_setParallelHash(unsigned _parallelHash);

	bool cuda_init(
	    size_t numDevices,
	    ethash_light_t _light,
	    uint8_t const* _lightData,
	    uint64_t _lightSize,
	    unsigned _deviceId,
	    bool _cpyToHost,
	    uint8_t*& hostDAG,
	    unsigned dagCreateDevice);

	void search(
	    uint8_t const* header,
	    uint64_t target,
	    bool _ethStratum,
	    uint64_t _startN,
	    const dev::eth::WorkPackage& w);

protected:
	void kick_miner() override;

private:
	atomic<bool> m_new_work = {false};

	void workLoop() override;

	bool init(const h256& seed);

	///Constants on GPU
	hash128_t* m_dag = nullptr;
	std::vector<hash64_t*> m_light;
	uint32_t m_dag_size = -1;
	uint32_t m_device_num;

	volatile search_results** m_search_buf;
	cudaStream_t*   m_streams;

	/// The local work size for the search
	static unsigned s_blockSize;
	/// The initial global work size for the searches
	static unsigned s_gridSize;
	/// The number of CUDA streams
	static unsigned s_numStreams;
	/// CUDA schedule flag
	static unsigned s_scheduleFlag;
	static unsigned s_parallelHash;
	static unsigned s_numInstances;
	static vector<int> s_devices;

	static bool s_eval;

};


}
}
