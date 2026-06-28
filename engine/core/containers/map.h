#pragma once

#include "alloc/alloc.h"
#include "common.h"

#define NTT_MAP_DEFAULT_BUCKETS 16

namespace ntt {

template <typename K, typename V>
struct KeyValuePair
{
	K key;
	V value;

	KeyValuePair(const K& k, const V& v)
		: key(k)
		, value(v)
	{
	}

	KeyValuePair(K&& k, V&& v)
		: key(static_cast<K&&>(k))
		, value(static_cast<V&&>(v))
	{
	}
};

template <typename K, typename V>
class Map
{
private:
	struct Bucket;

	struct Node
	{
		KeyValuePair<K, V> data;
		Bucket*			   pBucket;
		Pointer<Node>	   pNext;
	};

	struct Bucket
	{
		Pointer<Node> pHead;
		Pointer<Node> pTail;
		Map*		  pMap;
		u32			  bucketIndex;
		u32			  count;
	};

public:
	typedef u32 (*HashFunction)(const K&);

	struct Iterator
	{
	public:
		Iterator(Node* pNode)
			: m_pNode(pNode)
		{
		}

		Iterator(const Iterator& other)
			: m_pNode(other.m_pNode)
		{
		}

		Iterator(Iterator&& other) noexcept
			: m_pNode(other.m_pNode)
		{
			other.m_pNode = nullptr;
		}

		Iterator& operator++()
		{
			if (m_pNode != nullptr)
			{
				if (m_pNode->pNext != nullptr)
				{
					m_pNode = m_pNode->pNext.Get();
				}
				else
				{
					u32 newBucketIndex = m_pNode->pBucket->bucketIndex + 1;
					if (newBucketIndex >= m_pNode->pBucket->pMap->m_BucketCount)
					{
						m_pNode = nullptr; // Reached the end of the map
					}
					else
					{
						Bucket* pNextBucket = &m_pNode->pBucket->pMap->m_pData.Get()[newBucketIndex];
						m_pNode				= pNextBucket->pHead.Get();
					}
				}
			}
			return *this;
		}

		K& key() const
		{
			return m_pNode->data.key;
		}

		V& value() const
		{
			return m_pNode->data.value;
		}

		~Iterator() = default;

	private:
		friend class Map<K, V>;
		Node* m_pNode;
	};

public:
	Map(u32			 bucketCount  = NTT_MAP_DEFAULT_BUCKETS,
		HashFunction hashFunction = nullptr,
		IAllocator*	 pAllocator	  = nullptr)
		: m_BucketCount(bucketCount)
		, m_HashFunction(hashFunction)
		, m_pAllocator(pAllocator)
	{
		m_pData = (ALLOCATOR_SAFE(m_pAllocator)->Allocate((u32)sizeof(Bucket) * m_BucketCount)).Cast<Bucket>();

		for (u32 i = 0; i < m_BucketCount; ++i)
		{
			m_pData.Get()[i].pMap		 = this;
			m_pData.Get()[i].bucketIndex = i;
			m_pData.Get()[i].count		 = 0;
		}

		NTT_ASSERT_MSG(m_pData != nullptr, "Failed to allocate memory for Map.");
	}

	Map(const Map&) = delete;

	Map(Map&& other) noexcept
		: m_pData(other.m_pData)
		, m_BucketCount(other.m_BucketCount)
		, m_HashFunction(other.m_HashFunction)
		, m_pAllocator(other.m_pAllocator)
	{
		other.m_pData		 = nullptr;
		other.m_BucketCount	 = 0;
		other.m_HashFunction = nullptr;
		other.m_pAllocator	 = nullptr;
	}

	Map& operator=(const Map&) = delete;
	Map& operator=(Map&& other) noexcept
	{
		if (this != &other)
		{
			NTT_ASSERT_MSG(Clear() == RESULT_SUCCESS, "Failed to clear Map.");
			NTT_ASSERT_MSG(m_pData.Free() == RESULT_SUCCESS, "Failed to free memory for Map.");

			m_pData		   = other.m_pData;
			m_BucketCount  = other.m_BucketCount;
			m_HashFunction = other.m_HashFunction;
			m_pAllocator   = other.m_pAllocator;

			other.m_pData		 = nullptr;
			other.m_BucketCount	 = 0;
			other.m_HashFunction = nullptr;
			other.m_pAllocator	 = nullptr;
		}
		return *this;
	}

	~Map()
	{
		NTT_ASSERT_MSG(Clear() == RESULT_SUCCESS, "Failed to clear Map.");
		if (m_pData != nullptr)
		{
			NTT_ASSERT_MSG(m_pData.Free() == RESULT_SUCCESS, "Failed to free memory for Map.");
		}
	}

public:
	Result Insert(const K& key, V&& value) noexcept;
	V&	   operator[](const K& key);
	Result Clear();
	Result Remap(u32 newBucketCount);

public:
	friend struct Iterator;

private:
	Result InsertNode(const K& key, V&& value, Bucket* pBucket, u32 bucketIndex);

private:
	u32				m_BucketCount;
	Pointer<Bucket> m_pData;
	HashFunction	m_HashFunction;
	IAllocator*		m_pAllocator;
};

template <typename K, typename V>
Result Map<K, V>::Insert(const K& key, V&& value) noexcept
{
	NTT_ASSERT_MSG(m_HashFunction != nullptr, "Hash function must be provided for Map.");

	u32 hashValue	= m_HashFunction(key);
	u32 bucketIndex = hashValue % m_BucketCount;

	Bucket* pBucket = &m_pData.Get()[bucketIndex];
	return InsertNode(key, static_cast<V&&>(value), pBucket, bucketIndex);
}

template <typename K, typename V>
Result Map<K, V>::InsertNode(const K& key, V&& value, Bucket* pBucket, u32 bucketIndex)
{
	if (bucketIndex >= m_BucketCount)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	if (pBucket->pHead == nullptr)
	{
		Pointer<Node> pNode = (ALLOCATOR_SAFE(m_pAllocator)->Allocate(sizeof(Node))).Cast<Node>();
		if (pNode == nullptr)
		{
			return RESULT_OUT_OF_MEMORY;
		}

		pNode->data.key	  = key;
		pNode->data.value = static_cast<V&&>(value);
		pNode->pBucket	  = pBucket;
		pNode->pNext	  = nullptr;

		pBucket->pHead = pNode;
		pNode->pBucket = pBucket;
		pBucket->pTail = pNode;
	}
	else
	{
		Pointer<Node> pCurrent = pBucket->pHead;

		while (pCurrent != nullptr)
		{
			if (pCurrent->data.key == key)
			{
				// Key already exists, update the value
				pCurrent->data.value = static_cast<V&&>(value);
				return RESULT_SUCCESS;
			}
			pCurrent = pCurrent->pNext;
		}

		Pointer<Node> pNode = (ALLOCATOR_SAFE(m_pAllocator)->Allocate(sizeof(Node))).Cast<Node>();
		pNode->data.key		= key;
		pNode->data.value	= static_cast<V&&>(value);
		pNode->pNext		= nullptr;

		pCurrent		= pBucket->pTail;
		pCurrent->pNext = pNode;
		pNode->pBucket	= pBucket;
		pBucket->pTail	= pNode;
	}
	return RESULT_SUCCESS;
}

template <typename K, typename V>
V& Map<K, V>::operator[](const K& key)
{
	NTT_ASSERT_MSG(m_HashFunction != nullptr, "Hash function must be provided for Map.");
	u32 hashValue = m_HashFunction(key);

	u32		bucketIndex = hashValue % m_BucketCount;
	Bucket* pBucket		= &m_pData.Get()[bucketIndex];

	Pointer<Node> pCurrent = pBucket->pHead;

	while (pCurrent != nullptr)
	{
		if (pCurrent->data.key == key)
		{
			return pCurrent->data.value;
		}
		pCurrent = pCurrent->pNext;
	}

	NTT_UNREACHABLE();
	return *static_cast<V*>(nullptr); // This line is never reached, but added to avoid compiler warnings.
}

template <typename K, typename V>
Result Map<K, V>::Clear()
{
	for (u32 i = 0; i < m_BucketCount; ++i)
	{
		Bucket*		  pBucket  = &m_pData.Get()[i];
		Pointer<Node> pCurrent = pBucket->pHead;

		while (pCurrent != nullptr)
		{
			Pointer<Node> pNext = pCurrent->pNext;
			pCurrent.Get()->~Node(); // Call the destructor for the node
			NTT_ASSERT_MSG(pCurrent.Free() == RESULT_SUCCESS, "Failed to free memory for Node.");
			pCurrent = pNext;
		}

		pBucket->pHead = nullptr;
		pBucket->pTail = nullptr;
		pBucket->count = 0;
	}

	return RESULT_SUCCESS;
}

template <typename K, typename V>
Result Map<K, V>::Remap(u32 newBucketCount)
{
	if (newBucketCount <= m_BucketCount)
	{
		return RESULT_SUCCESS;
	}

	Pointer<Bucket> pNewData =
		(ALLOCATOR_SAFE(m_pAllocator)->Allocate((u32)sizeof(Bucket) * newBucketCount)).Cast<Bucket>();

	for (u32 i = 0; i < newBucketCount; ++i)
	{
		pNewData.Get()[i].pMap		  = this;
		pNewData.Get()[i].bucketIndex = i;
		pNewData.Get()[i].count		  = 0;
	}

	for (u32 i = 0; i < m_BucketCount; ++i)
	{
		if (m_pData.Get()[i].pHead != nullptr)
		{
			Pointer<Node> pCurrent = m_pData.Get()[i].pHead;

			while (pCurrent != nullptr)
			{
				u32		hashValue	   = m_HashFunction(pCurrent->data.key);
				u32		newBucketIndex = hashValue % newBucketCount;
				Bucket* pNewBucket	   = &pNewData.Get()[newBucketIndex];

				Result result =
					InsertNode(pCurrent->data.key, static_cast<V&&>(pCurrent->data.value), pNewBucket, newBucketIndex);
				if (result != RESULT_SUCCESS)
				{
					return result;
				}

				pCurrent = pCurrent->pNext;
			}
		}
	}

	NTT_ASSERT_MSG(Clear() == RESULT_SUCCESS, "Failed to clear Map.");
	NTT_ASSERT_MSG(m_pData.Free() == RESULT_SUCCESS, "Failed to free memory for Map.");

	m_pData		  = pNewData;
	m_BucketCount = newBucketCount;

	return RESULT_SUCCESS;
}

} // namespace ntt