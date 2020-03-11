#pragma once
#include <assert.h>




template <class _Type>
class nvMatrix
{
    _Type **m;

    size_t rows, cols;

public:

    nvMatrix()
    { 
        rows = 0;
        cols = 0;
        m = 0;
    }

    // copy constructor
    nvMatrix(const nvMatrix & v)
    {         
        rows = 0;
        cols = 0;
        m = 0;

        resize(v.width(), v.height());

        for(size_t i = 0; i < v.rows; i++)
        {
            for(size_t j = 0; j<v.cols; j++)
                m[i][j] = v.m[i][j];
        }
    }

    nvMatrix(size_t width, size_t height)
    { 
        m = 0;
        rows = 0;
        cols = 0;
        resize(width, height);
    }
	~nvMatrix()
	{
		tvfree();
	}

    _Type& operator [] ( size_t i) 
    {
        size_t r = i / cols;
        size_t c = i % cols;

#if _DEBUG
        assert(r < rows);
        assert(c < cols);
#endif
        return m[r][c]; 
    };  


    _Type * pixels( size_t i = 0) 
    {

        size_t r = i / cols;
        size_t c = i % cols;

#if _DEBUG
        assert(r < rows);
        assert(c < cols);
#endif
        return &m[r][c]; 
    };  


    _Type & operator () (const size_t &r, const size_t &c) const
    {
#if _DEBUG
        assert(r < rows);
        assert(c < cols);
#endif
        return m[r][c]; 
    }

    _Type & operator () (const size_t &r, const size_t &c) 
    {
#if _DEBUG
        assert(r < rows);
        assert(c < cols);
#endif
        return m[r][c]; 
    }

    _Type * pixelsRC( size_t r,  size_t c) 
    {
#if _DEBUG
        assert(r < rows);
        assert(c < cols);
#endif
        return &m[r][c]; 
    };  

    _Type * pixelsXY( size_t x,  size_t y) 
    {
#if _DEBUG
        assert(y < rows);
        assert(x < cols);
#endif
        return &m[y][x]; 
    };  


    _Type * pixelsYX( size_t y,  size_t x) 
    {
#if _DEBUG
        assert(y < rows);
        assert(x < cols);
#endif
        return &m[y][x]; 
    };  


    _Type * pixelsXY_wrapped(int x, int y)
    {
        y = iModulo(y, (int)rows);
        x = iModulo(x, (int)cols);

        return &m[y][x]; 
    }

    size_t width() const
    {
        return cols;
    }
    size_t height() const
    {
        return rows;
    }


    void tvfree()
    {
        if (m)
        {
            for(size_t i = 0; i < rows; i++)
                delete [] m[i];

            delete [] m;
            m = 0;
            rows = 0;
            cols = 0;
        }
    }

    void tvallocate(size_t r, size_t c)
    {
		assert(m == NULL);

        rows = r;
        cols = c;
        if (r == 0 || c == 0)
            return;

        m = new _Type *[r];
        
        
        for(size_t i = 0; i < r; i++)
        {
            m[i] = new _Type [c];
        }
    }

    nvMatrix & operator = ( const nvMatrix& v )
    {
        resize(v.width(), v.height());

        for(size_t i = 0; i < v.rows; i++)
        {
            for(size_t j = 0; j<v.cols; j++)
                m[i][j] = v.m[i][j];
        }
        return *this; 
    }


    void SetToZero()
    {
        for(size_t i = 0; i < rows; i++)
        {
            for(size_t j = 0; j<cols; j++)
                m[i][j].SetToZero();
        }
    }


  

    // destructive
    void resize(size_t width, size_t height)
    {
        if (height != rows || width != cols)
        {
            tvfree ();

            tvallocate(height, width);
        }
    }


    void Release()
    {
        tvfree ();
    }
    void clear()
    {
        tvfree();
    }

    size_t size() const
    {
        return rows * cols;
    }



    void FlipTopToBottom()
    {
        _Type * swap = new _Type[ cols];

        size_t row;

        //_Type * end_row;
        //_Type * start_row;
        size_t end_row;    //!AB 64-bit warning
        size_t start_row;  //!AB 64-bit warning

        //!AB not used: size_t len = sizeof(_Type) * cols;

        for( row = 0; row < rows / 2; row ++ )
        {
            /*end_row =   &m[( rows - row - 1) ][0];
            start_row = &m[ row ][0];

            // copy row toward end of image into temporary swap buffer
            memcpy( swap, end_row, len );

            // copy row at beginning to row at end
            memcpy( end_row, start_row, len );

            // copy old bytes from row at end (in swap) to row at beginning
            memcpy( start_row, swap, len );*/

            end_row =   rows - row - 1;
            start_row =  row ;

            // copy row toward end of image into temporary swap buffer
            //memcpy( swap, end_row, len );
			{ //!AB, need for VC6
            for(size_t i = 0; i < cols; i++)
                swap[i] = m[end_row][i];
			}

            // copy row at beginning to row at end
            //memcpy( end_row, start_row, len );
            { //!AB, need for VC6
			for(size_t i = 0; i < cols; i++)
                m[end_row][i] = m[start_row][i];
			}

            // copy old bytes from row at end (in swap) to row at beginning
            //memcpy( start_row, swap, len );
            for(size_t i = 0; i < cols; i++)
                m[start_row][i] = swap[i];

        }

        delete [] swap;
    }


    void Scale(_Type s)
    {
        for(size_t i = 0; i < rows; i++)
        {
            for(size_t j = 0; j<cols; j++)
                m[i][j] *= s;
        }
    }

    void Bias(_Type s)
    {
        for(size_t i = 0; i < rows; i++)
        {
            for(size_t j = 0; j<cols; j++)
                m[i][j] += s;
        }
    }
     
    void dot(_Type w)
    {
        for(size_t i = 0; i < rows; i++)
        {
            for(size_t j = 0; j<cols; j++)
                m[i][j].dot(w);
        }
    }
    


    void Clamp(_Type low, _Type hi)
    {
        for(size_t i = 0; i < rows; i++)
        {
            for(size_t j = 0; j<cols; j++)
            {
                m[i][j].Clamp(low, hi);
            }
        }
    }

    void Wrap(_Type low, _Type hi)
    {
        // to do
        for(size_t i = 0; i < rows; i++)
        {
            for(size_t j = 0; j<cols; j++)
            {

                m[i][j].Wrap(low, hi);
            }
        }
    }

};

/*
#include <vector>

template <class T>
class nvVector : public std::vector<T>
{
	typedef std::vector<T> base; //!AB, need for VC6
public:

	void resize(size_t newSize)
	{
		// Hack to emulate nvVector behaviour.
		nvVector tmp;
		((std::vector<T> &)tmp).resize(newSize);
		std::swap(*this, tmp);
	}

	void realloc(size_t newSize)
	{
		base::resize(newSize); //!AB, need for VC6
	}
};

template <class _Type>
class nvVector
{
    _Type *m_data;

    size_t allocated_size;
    size_t current_size;

public:

    nvVector()
    { 
        m_data = 0;
        current_size = 0;
        allocated_size = 0;
    }

    nvVector(const nvVector<_Type> & other)
    {
        resize(other.size());

        for(size_t i = 0; i < other.size(); i++)
        {
            m_data[i] = other.m_data[i];
        }
    }

    ~nvVector()
    {
        Release();
    }

    static void tvfree(_Type * & ptr)
    {
        if (ptr)
        {
            delete [] ptr;
            ptr = 0;
        }
    }

    static _Type * tvallocate(size_t elements)
    {
        return new _Type[elements];
    }

    nvVector & operator = ( const nvVector& v )
    {
        resize(v.size());

        for(size_t i = 0; i < v.size(); i++)
        {
            m_data[i] = v.m_data[i];
        }
        return *this; 
    }


    void FirstAllocation()
    {
        // start with 256 entries

        tvfree (m_data);

        allocated_size = 256;
        current_size = 0;
        m_data = tvallocate(allocated_size);
    }



    void resize(size_t newSize)
    {
        if (newSize != allocated_size)
        {
            allocated_size = newSize;

            tvfree (m_data);

            m_data = tvallocate(allocated_size);
        }

        current_size = newSize;
    }

    // keep old contents
    void realloc(size_t newSize)
    {

        if (newSize != allocated_size)
        {
            _Type * oldData = new _Type[current_size];
            size_t oldSize = current_size;

            for(size_t i = 0; i < current_size; i++)
            {
                oldData[i] = m_data[i];
            }


            allocated_size = newSize;

            tvfree (m_data);

            m_data = tvallocate(allocated_size);

            size_t minSize;
            if (oldSize < newSize)
                minSize = oldSize;
            else
                minSize = newSize;

            for(size_t i = 0; i < minSize; i++)
            {
                m_data[i] = oldData[i];
            }
        }

        current_size = newSize;
    }

    void push_back(_Type item)
    {
        if (allocated_size == 0)
        {
            FirstAllocation();
        }
        else if (current_size >= allocated_size)
        {
            allocated_size = allocated_size * 2;

            _Type *temp = tvallocate(allocated_size);

            // copy old data to new area
            for(size_t i = 0; i< current_size; i++)
                temp[i] = m_data[i];

            tvfree (m_data);

            m_data = temp;
        }

        
        m_data[current_size] = item;
        current_size++;
     }

    // indexing

    _Type& operator [] ( size_t i) 
    {
#ifdef _DEBUG
        assert(i < current_size);
        assert(current_size <= allocated_size);
#endif
        return m_data[i]; 
    };  
    
    const _Type& operator[](size_t i) const 
    { 
#ifdef _DEBUG
        assert(i < current_size);
        assert(current_size <= allocated_size);
#endif
        return m_data[i];
    }

    void Release()
    {
        tvfree (m_data);

        current_size = 0;
        allocated_size = 0;
    }
    void clear()
    {
        Release();
    }

    size_t size() const
    {
        return current_size;
    }

};
*/

#include <new>

template<typename T>
class nvVector {
public:
	
	/** Ctor. */
	nvVector() : m_buffer(NULL), m_size(0), m_buffer_size(0) {
	}

	/** Copy ctor. */
	nvVector( const nvVector & a ) : m_buffer(NULL), m_size(0), m_buffer_size(0) {
		copy( a.m_buffer, a.m_size); 
	}


	/** Ctor that initializes the array with the given elements. */
	nvVector( const T * ptr, size_t num ) : m_buffer(NULL), m_size(0), m_buffer_size(0) {
		copy( ptr, num); 
	}


	/** Dtor. */
	~nvVector() {
		clear();
		allocate(0);
	}


	/** Const and save array access. */
	const T & operator[]( size_t index ) const {
		assert( index < m_size );
		return m_buffer[index];
	}

	/** Safe array access. */
	T & operator[] ( size_t index ) {
		assert( index < m_size );
		return m_buffer[index];
	}


	/** Get array size. */
	size_t size() const { return m_size; }


	/** Push an element at the end of the array. */
	void push_back( const T & val ) {
		// DO NOT pass elements of your own vector into
		// push_back()!  Since we're using references,
		// resize() may munge the element storage!
		assert( &val < &m_buffer[0] || &val > &m_buffer[m_size] );
		
		int	new_size = m_size + 1;
		resize( new_size );
		m_buffer[new_size-1] = val;
	}
	
	/** Pop and return element at the end of the array. */
	void pop_back() {
		assert( m_size > 0 );
		resize( m_size-1 );
	}

	/** Get back element. */
	const T & back() const {
		assert( m_size > 0 );
		return m_buffer[m_size-1];
	}

	/** Get back element. */
	T & back() {
		assert( m_size > 0 );
		return m_buffer[m_size-1];
	}

	/** Get back element. */
	const T & front() const {
		assert( m_size > 0 );
		return m_buffer[0];
	}

	/** Get back element. */
	T & front() {
		assert( m_size > 0 );
		return m_buffer[0];
	}


	// nvVector semantics: realloc preserves contents, resize does not.
	void realloc( size_t new_size ) {
		resize(new_size);
	}


	/** Resize the array preserving existing elements. */
	void resize( size_t new_size ) {
		size_t i;
		size_t old_size = m_size;
		m_size = new_size;
		
		// Destruct old elements (if we're shrinking).
		for( i = new_size; i < old_size; i++ ) {
			(m_buffer+i)->~T();							// Explicit call to the destructor
		}
		
		if( m_size == 0 ) {
			//allocate(0);	// Don't shrink automatically.
		}
		else if( m_size <= m_buffer_size && m_size > m_buffer_size >> 1) {
			// don't compact yet.
			assert(m_buffer != NULL);
		}
		else {
			size_t new_buffer_size;
			if( m_buffer_size == 0 ) {
				// first allocation
				new_buffer_size = m_size;
			}
			else {
				// growing
				new_buffer_size = m_size + (m_size >> 2);
			}
			allocate( new_buffer_size );
		}
		
		// Call default constructors
		for( i = old_size; i < new_size; i++ ) {
			new(m_buffer+i) T();							// placement new
		}
	}


	/**
	 * Resize the array preserving existing elements and initializing the
	 * new ones with the given value.
	 */
	void resize( size_t new_size, const T &elem ) {
		size_t i;
		size_t old_size = m_size;
		m_size = new_size;
		
		// Destruct old elements (if we're shrinking).
		for( i = new_size; i < old_size; i++ ) {
			(m_buffer+i)->~T();							// Explicit call to the destructor
		}
		
		if( m_size == 0 ) {
			//allocate(0);	// Don't shrink automatically.
		}
		else if( m_size <= m_buffer_size && m_size > m_buffer_size >> 1 ) {
			// don't compact yet.
			assert(m_buffer != NULL);
		}
		else {
			size_t new_buffer_size;
			if( m_buffer_size == 0 ) {
				// first allocation
				new_buffer_size = m_size;
			}
			else {
				// growing
				new_buffer_size = m_size + (m_size >> 2);
			}
			allocate( new_buffer_size );
		}
		
		// Call copy constructors
		for( i = old_size; i < new_size; i++ ) {
			new(m_buffer+i) T( elem );					// placement new
		}
	}

	/** Clear the buffer. */
	void clear() {
		resize(0);
	}

	/** Shrink the allocated array. */
	void shrink() {
		if( m_size < m_buffer_size) {
			allocate(m_size);
		}
	}
	
	/** Preallocate space. */
	void reserve( size_t desired_size ) {
		if( desired_size > m_buffer_size ) {
			allocate( desired_size );
		}
	}
	

	/** Assignment operator. */
	void operator=( const nvVector & a ) {
		copy( a.m_buffer, a.m_size );
	}


private:

	/** Change buffer size. */
	void allocate( size_t rsize ) {
		
		m_buffer_size = rsize;
		
		// free the buffer.
		if( m_buffer_size == 0 ) {
			if( m_buffer ) {
				free( m_buffer );
				m_buffer = NULL;
			}
		}
		
		// realloc the buffer
		else {
			if( m_buffer ) m_buffer = (T *) ::realloc( m_buffer, sizeof(T) * m_buffer_size );
			else m_buffer = (T *) malloc( sizeof(T) * m_buffer_size );
		}
	}

	/** Copy memory to our array. Resizes the array if needed. */
	void copy( const T * ptr, size_t num ) {
		resize( num );
		for( size_t i = 0; i < m_size; i++ ) {
			m_buffer[i] = ptr[i];
		}
	}

private:

	T * m_buffer;
	size_t m_size;
	size_t m_buffer_size;

};
