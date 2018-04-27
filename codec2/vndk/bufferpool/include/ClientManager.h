/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_HARDWARE_MEDIA_BUFFERPOOL_V1_0_CLIENTMANAGER_H
#define ANDROID_HARDWARE_MEDIA_BUFFERPOOL_V1_0_CLIENTMANAGER_H

#include <android/hardware/media/bufferpool/1.0/IClientManager.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <memory>
#include <BufferPoolTypes.h>

namespace android {
namespace hardware {
namespace media {
namespace bufferpool {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::media::bufferpool::V1_0::IAccessor;
using ::android::hardware::media::bufferpool::V1_0::ResultStatus;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct ClientManager : public IClientManager {
    // Methods from ::android::hardware::media::bufferpool::V1_0::IClientManager follow.
    Return<void> registerSender(const sp<::android::hardware::media::bufferpool::V1_0::IAccessor>& bufferPool, registerSender_cb _hidl_cb) override;

    /** Gets an instance. */
    static sp<ClientManager> getInstance();

    /**
     * Creates a local connection with a newly created buffer pool.
     *
     * @param allocator     for new buffer allocation.
     * @param pConnectionId Id of the created connection. This is
     *                      system-wide unique.
     *
     * @return OK when a buffer pool and a local connection is successfully
     *         created.
     *         NO_MEMORY when there is no memory.
     *         CRITICAL_ERROR otherwise.
     */
    ResultStatus create(const std::shared_ptr<BufferPoolAllocator> &allocator,
                        ConnectionId *pConnectionId);

    /**
     * Closes the specified connection.
     *
     * @param connectionId  The id of the connection.
     *
     * @return OK when the connection is closed.
     *         NOT_FOUND when the specified connection was not found.
     *         CRITICAL_ERROR otherwise.
     */
    ResultStatus close(ConnectionId connectionId);

    /**
     * Allocates a buffer from the specified connection.
     *
     * @param connectionId  The id of the connection.
     * @param params        The allocation parameters.
     * @param buffer        The allocated buffer.
     *
     * @return OK when a buffer was allocated successfully.
     *         NOT_FOUND when the specified connection was not found.
     *         NO_MEMORY when there is no memory.
     *         CRITICAL_ERROR otherwise.
     */
    ResultStatus allocate(ConnectionId connectionId,
                          const std::vector<uint8_t> &params,
                          std::shared_ptr<_C2BlockPoolData> *buffer);

    /**
     * Receives a buffer for the transaction.
     *
     * @param connectionId  The id of the receiving connection.
     * @param transactionId The id for the transaction.
     * @param bufferId      The id for the buffer.
     * @param timestampUs   The timestamp of the buffer is being sent.
     * @param buffer        The received buffer.
     *
     * @return OK when a buffer was received successfully.
     *         NOT_FOUND when the specified connection was not found.
     *         NO_MEMORY when there is no memory.
     *         CRITICAL_ERROR otherwise.
     */
    ResultStatus receive(ConnectionId connectionId,
                         TransactionId transactionId,
                         BufferId bufferId,
                         int64_t timestampUs,
                         std::shared_ptr<_C2BlockPoolData> *buffer);

    /**
     * Posts a buffer transfer transaction to the buffer pool. Sends a buffer
     * to other remote clients(connection) after this call has been succeeded.
     *
     * @param connectionId  The id of the sending connection.
     * @param receiverId    The id of the receiving connection.
     * @param buffer        to transfer
     * @param transactionId Id of the transfer transaction.
     * @param timestampUs   The timestamp of the buffer transaction is being
     *                      posted.
     *
     * @return OK when a buffer transaction was posted successfully.
     *         NOT_FOUND when the sending connection was not found.
     *         CRITICAL_ERROR otherwise.
     */
    ResultStatus postSend(ConnectionId connectionId,
                          ConnectionId receiverId,
                          const std::shared_ptr<_C2BlockPoolData> &buffer,
                          TransactionId *transactionId,
                          int64_t *timestampUs);

    /** Gets a buffer pool for the specified connection.
     *
     * @param connectionId  The id of the connection.
     * @param accessor      The buffer pool for the specified connection.
     * @return OK when a buffer pool was found for the connection.
     *         NOT_FOUND when the specified connection was not found.
     *         CRITICAL_ERROR otherwise.
     */
    ResultStatus getAccessor(ConnectionId connectionId,
                             sp<IAccessor> *accessor);

    /** Destructs the manager of buffer pool clients.  */
    ~ClientManager();
private:
    static sp<ClientManager> sInstance;
    static std::mutex sInstanceLock;

    class Impl;
    const std::unique_ptr<Impl> mImpl;

    ClientManager();
};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" IClientManager* HIDL_FETCH_IClientManager(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace bufferpool
}  // namespace media
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_MEDIA_BUFFERPOOL_V1_0_CLIENTMANAGER_H
