/*****************************************************************************
*                                                                            *
* PROJECT_NAME             OPC UA C++ Server & Client SDK           *
*                                                                            *
* VERSION                  6.00.0                                            *
*                                                                            *
* DATE                     13.6.2022                                        *
*                                                                            *
*                                                                            *
* NOTICE:                                                                    *
*   This code and information is provided "as is" without warranty of        *
*   any kind, either expressed or implied, including but not limited         *
*   to the implied warranties of merchantability and/or fitness for a        *
*   particular purpose.                                                      *
*****************************************************************************/
#ifndef _UTIL_NATIVE_LINKED_QUEUE_H_
#define _UTIL_NATIVE_LINKED_QUEUE_H_

	template<class T> class Util_DefaultAccessor
	{
	public:
		static T* &next(T* t)
		{
			return t->pNext;
		}
		static T* &prev(T* t)
		{
			return t->pPrev;
		}
	};
	template<class T> class  Util_AltAccessor
	{
	public:
		static T* &next(T* t)
		{
			return t->pAltNext;
		}
		static T* &prev(T* t)
		{
			return t->pAltPrev;
		}
	};

	template<class T, class A=Util_DefaultAccessor<T> > class  Util_NativeLinkedQueue
	{
	public:
		Util_NativeLinkedQueue()
		{
			m_pFront = 0;
			m_pTail  = 0;
			m_size   = 0;
		}

		void clear()
		{
			m_pFront = 0;
			m_pTail  = 0;
			m_size   = 0;
		}

		T* front() const
		{
			_ASSERTION( (m_size==0 && !m_pFront && !m_pTail) ||
						(m_size==1 && m_pFront && m_pFront==m_pTail && !prev(m_pFront) && !next(m_pTail)) ||
						(m_size>1 && m_pFront && m_pTail && m_pFront!=m_pTail && !prev(m_pFront) && !next(m_pTail)
						&& next(m_pFront) && next(m_pFront)!=m_pFront && prev(next(m_pFront))==m_pFront
						&& prev(m_pTail) && prev(m_pTail)!=m_pTail && next(prev(m_pTail))==m_pTail), _T("list corrupted"));
			return m_pFront;
		}

		T* tail() const
		{
			_ASSERTION( (m_size==0 && !m_pFront && !m_pTail) ||
						(m_size==1 && m_pFront && m_pFront==m_pTail && !prev(m_pFront) && !next(m_pTail)) ||
						(m_size>1 && m_pFront && m_pTail && m_pFront!=m_pTail && !prev(m_pFront) && !next(m_pTail)
						&& next(m_pFront) && next(m_pFront)!=m_pFront && prev(next(m_pFront))==m_pFront
						&& prev(m_pTail) && prev(m_pTail)!=m_pTail && next(prev(m_pTail))==m_pTail), _T("list corrupted"));
			return m_pTail;
		}

		/*! @brief Removes the given item from the linked list and returns true if it was removed,
		false if it was not in the list. */
		bool unlink(T* pT)
		{
			_ASSERTION( (m_size==0 && !m_pFront && !m_pTail) ||
						(m_size==1 && m_pFront && m_pFront==m_pTail && !prev(m_pFront) && !next(m_pTail)) ||
						(m_size>1 && m_pFront && m_pTail && m_pFront!=m_pTail && !prev(m_pFront) && !next(m_pTail)
						&& next(m_pFront) && next(m_pFront)!=m_pFront && prev(next(m_pFront))==m_pFront
						&& prev(m_pTail) && prev(m_pTail)!=m_pTail && next(prev(m_pTail))==m_pTail), _T("list corrupted"));
			if(pT)
			{
				_ASSERTION((!next(pT) && m_pTail==pT) || (next(pT) && next(pT)!=pT && prev(next(pT))==pT), _T("list corrupted"));
				_ASSERTION((!prev(pT) && m_pFront==pT) || (prev(pT) && prev(pT)!=pT && next(prev(pT))==pT), _T("list corrupted"));

				if(next(pT))
					prev(next(pT)) = prev(pT);

				if(m_pFront == pT)
					m_pFront = next(pT);

				if(prev(pT))
					next(prev(pT)) = next(pT);

				if(m_pTail == pT)
					m_pTail = prev(pT);

				next(pT) = NULL;
				prev(pT) = NULL;

				_ASSERTION(m_size > 0, _T("Underflow in list."));
				m_size--;
				return true;
			}
			return false;
		}

		/*! @brief Pushes the given item to the list's front end */
		void pushToFront(T* pT)
		{
			if(unlink(pT))
				push_front(pT);
		}

		/*! @brief Pushes the given item to the list's back end */
		void pushToBack(T* pT)
		{
			if(unlink(pT))
				push_back(pT);
		}

		void appendAndClear(Util_NativeLinkedQueue* pQueue)
		{
			_ASSERTION( (m_size==0 && !m_pFront && !m_pTail) ||
						(m_size==1 && m_pFront && m_pFront==m_pTail && !prev(m_pFront) && !next(m_pTail)) ||
						(m_size>1 && m_pFront && m_pTail && m_pFront!=m_pTail && !prev(m_pFront) && !next(m_pTail)
						&& next(m_pFront) && next(m_pFront)!=m_pFront && prev(next(m_pFront))==m_pFront
						&& prev(m_pTail) && prev(m_pTail)!=m_pTail && next(prev(m_pTail))==m_pTail), _T("list corrupted"));
			_ASSERTION(pQueue && pQueue!=this, _T("invalid parameter"));
			if(pQueue->front())
			{
				if(!m_pFront)
					m_pFront = pQueue->m_pFront;
				if(m_pTail)
					next(m_pTail) = pQueue->m_pFront;
				if(pQueue->m_pFront)
					prev(pQueue->m_pFront) = m_pTail;
				if(pQueue->m_pTail)
					m_pTail = pQueue->m_pTail;
				m_size += pQueue->m_size;

				pQueue->clear();
			}
		}

		/*! @brief Adds the given item to the list's back end */
		void push_back(T* pT)
		{
			_ASSERTION( (m_size==0 && !m_pFront && !m_pTail) ||
						(m_size==1 && m_pFront && m_pFront==m_pTail && !prev(m_pFront) && !next(m_pTail)) ||
						(m_size>1 && m_pFront && m_pTail && m_pFront!=m_pTail && !prev(m_pFront) && !next(m_pTail)
						&& next(m_pFront) && next(m_pFront)!=m_pFront && prev(next(m_pFront))==m_pFront
						&& prev(m_pTail) && prev(m_pTail)!=m_pTail && next(prev(m_pTail))==m_pTail), _T("list corrupted"));
			_ASSERTION(pT && pT!=m_pFront && pT!=m_pTail, _T("invalid parameter"));
			if(m_pTail) // The list is not empty
			{
				next(m_pTail) = pT;
				prev(pT) = m_pTail;
				next(pT) = 0;
				m_pTail = pT;
			}
			else
			{
				m_pTail = m_pFront = pT;
				next(pT) = prev(pT) = 0;
			}
			m_size++;
		}

		/*! @brief Adds the given item to the list's front end */
		void push_front(T* pT)
		{
			_ASSERTION( (m_size==0 && !m_pFront && !m_pTail) ||
						(m_size==1 && m_pFront && m_pFront==m_pTail && !prev(m_pFront) && !next(m_pTail)) ||
						(m_size>1 && m_pFront && m_pTail && m_pFront!=m_pTail && !prev(m_pFront) && !next(m_pTail)
						&& next(m_pFront) && next(m_pFront)!=m_pFront && prev(next(m_pFront))==m_pFront
						&& prev(m_pTail) && prev(m_pTail)!=m_pTail && next(prev(m_pTail))==m_pTail), _T("list corrupted"));
			_ASSERTION(pT && pT!=m_pFront && pT!=m_pTail, _T("invalid parameter"));
			if(m_pFront) // The list is not empty
			{
				prev(m_pFront) = pT;
				next(pT) = m_pFront;
				prev(pT) = 0;
				m_pFront = pT;
			}
			else
			{
				m_pFront = m_pTail = pT;
				next(pT) = prev(pT) = 0;
			}
			m_size++;
		}


		T* pop_front()
		{
			T* ret = 0;
			_ASSERTION( (m_size==0 && !m_pFront && !m_pTail) ||
						(m_size==1 && m_pFront && m_pFront==m_pTail && !prev(m_pFront) && !next(m_pTail)) ||
						(m_size>1 && m_pFront && m_pTail && m_pFront!=m_pTail && !prev(m_pFront) && !next(m_pTail)
						&& next(m_pFront) && next(m_pFront)!=m_pFront && prev(next(m_pFront))==m_pFront
						&& prev(m_pTail) && prev(m_pTail)!=m_pTail && next(prev(m_pTail))==m_pTail), _T("list corrupted"));
			if(m_pFront)
			{
				ret = m_pFront;
				m_pFront = next(m_pFront);
				if(m_pFront)
				{
					prev(m_pFront) = 0;
				}
				else
				{
					m_pTail = 0;
				}

				_ASSERTION(m_size > 0, _T("Underflow in list."));
				m_size--;
			}
			return ret;
		}

		T* pop_back()
		{
			T* ret = 0;
			_ASSERTION( (m_size==0 && !m_pFront && !m_pTail) ||
						(m_size==1 && m_pFront && m_pFront==m_pTail && !prev(m_pFront) && !next(m_pTail)) ||
						(m_size>1 && m_pFront && m_pTail && m_pFront!=m_pTail && !prev(m_pFront) && !next(m_pTail)
						&& next(m_pFront) && next(m_pFront)!=m_pFront && prev(next(m_pFront))==m_pFront
						&& prev(m_pTail) && prev(m_pTail)!=m_pTail && next(prev(m_pTail))==m_pTail), _T("list corrupted"));
			if(m_pTail)
			{
				ret = m_pTail;
				m_pTail = prev(m_pTail);
				if(m_pTail)
				{
					next(m_pTail) = 0;
				}
				else
				{
					m_pFront = 0;
				}

				_ASSERTION(m_size > 0, _T("Underflow in list."));
				m_size--;
			}
			return ret;
		}

		size_t size() const
		{
			return m_size;
		}

		bool empty() const
		{
			return size() == 0;
		}

		/*! @brief std::list emulation */
		class iterator
		{
		public:
			iterator(){}
			T* operator*()
			{
				return pos;
			}
			iterator &operator++()
			{
				_ASSERTION(pos, _T("invalid list position"));
				_ASSERTION(!next(pos) || (next(pos)!=pos && prev(next(pos))==pos), _T("list corrupted"));
				pos = next(pos);
				return *this;
			}
			iterator operator++(int)
			{
				iterator result = *this;
				_ASSERTION(pos, _T("invalid list position"));
				_ASSERTION(!next(pos) || (next(pos)!=pos && prev(next(pos))==pos), _T("list corrupted"));
				pos = next(pos);
				return result;
			}
			bool operator==(const iterator &other) const
			{ return pos==other.pos; }
			bool operator!=(const iterator &other) const
			{ return pos!=other.pos; }
		private:
			friend class Util_NativeLinkedQueue;
			explicit iterator(T* where)
			{
				pos = where;
			}
			T* pos;
		};
		typedef iterator const_iterator;

		iterator begin()
		{
			return iterator(front());
		}

		iterator end()
		{
			return iterator(0);
		}

		const_iterator begin() const
		{
			return const_iterator(front());
		}

		const_iterator end() const
		{
			return const_iterator(0);
		}

		void erase(iterator &where)
		{
			unlink(*where);
		}

		void remove(T* pT)
		{
			unlink(pT);
		}

	private:
		T*     m_pFront;
		T*     m_pTail;
		size_t m_size;

		static T* &next(T* t)
		{
			return A::next(t);
		}

		static T* &prev(T* t)
		{
			return A::prev(t);
		}
	};

#endif
