/*
 * \file: sq_object_pool.h
 * \brief: Created by hushouguo at Jun 21 2013 16:52:04
 */
 
#ifndef __SQ_OBJECT_POOL_H__
#define __SQ_OBJECT_POOL_H__

#define OBJECT_BLOCK_NUMBER			16

template <typename OBJECT>
	class sq_object_pool
{
	public:
		sq_object_pool() : free_list(NULL), mem_list(NULL)
		{}
		~sq_object_pool()
		{
			while (mem_list)
			{
				struct _M_Node* next_node = mem_list->next;
				delete []mem_list;
				mem_list = next_node;
			}
		}
	public:
		OBJECT* allocate()
		{
			if (!free_list) { allocate_free_nodes(); }
			OBJECT* o = free_list->o;
			free_list = free_list->next;
			return Constructor(o);
		}
		template <typename P> OBJECT* allocate(P p)
		{
			if (!free_list) { allocate_free_nodes(); }
			OBJECT* o = free_list->o;
			free_list = free_list->next;
			return Constructor(o, p);
		}
		template <typename P1, typename P2> OBJECT* allocate(P1 p1, P2 p2)
		{
			if (!free_list) { allocate_free_nodes(); }
			OBJECT* o = free_list->o;
			free_list = free_list->next;
			return Constructor(o, p1, p2);
		}
		template <typename P1, typename P2, typename P3> OBJECT* allocate(P1 p1, P2 p2, P3 p3)
		{
			if (!free_list) { allocate_free_nodes(); }
			OBJECT* o = free_list->o;
			free_list = free_list->next;
			return Constructor(o, p1, p2, p3);
		}
		template <typename P1, typename P2, typename P3, typename P4> OBJECT* allocate(P1 p1, P2 p2, P3 p3, P4 p4)
		{
			if (!free_list) { allocate_free_nodes(); }
			OBJECT* o = free_list->o;
			free_list = free_list->next;
			return Constructor(o, p1, p2, p3, p4);
		}
		template <typename P1, typename P2, typename P3, typename P4, typename P5> OBJECT* allocate(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			if (!free_list) { allocate_free_nodes(); }
			OBJECT* o = free_list->o;
			free_list = free_list->next;
			return Constructor(o, p1, p2, p3, p4, p5);
		}
		void release(OBJECT*& o)
		{
			o->~OBJECT();
			this->free(o);
		}
	private:
		struct _M_Node
		{
			struct _M_Node* next;
			OBJECT o[0];
		};
		struct _M_Node * free_list, * mem_list;

		void allocate_free_nodes()
		{
			int node_size = sizeof(OBJECT) + sizeof(struct _M_Node*);
			char* p = new char[OBJECT_BLOCK_NUMBER * node_size];
			((struct _M_Node*)p)->next = mem_list;
			mem_list = (struct _M_Node*)p;
			for (int n = 1; n < OBJECT_BLOCK_NUMBER; ++n)
			{
				struct _M_Node* node = (struct _M_Node*)(p + n * node_size);
				node->next = free_list;
				free_list = node;
			}
		}
		void free(OBJECT*& o)
		{
			struct _M_Node* node = (struct _M_Node*)((unsigned long)o - sizeof(struct _M_Node*));
			node->next = free_list;
			free_list = node;
		}
};	

#endif
