import sys

def add_new_page_to_memory_pages(page, memory_pages, memory_pages_count, page_faults_count):
	memory_pages.append(page)
	return memory_pages_count + 1, page_faults_count + 1

def raise_runtime_error_memory_pages_count_being_greater_than_memory_maximum_pages_count():
	raise RuntimeError('the current number of pages in the memory is greater than the memory\'s capacity')

def FIFO(memory_maximum_pages_count, pages):
	memory_pages_count = 0
	memory_pages = []
	page_faults_count = 0
	fifo_index = 0
	for page in pages:
		if page not in memory_pages:
			if memory_pages_count < memory_maximum_pages_count: # memory is not full and thus no page replacement is necessary.
				memory_pages_count, page_faults_count = add_new_page_to_memory_pages(page, memory_pages, memory_pages_count, page_faults_count)
			elif memory_pages_count == memory_maximum_pages_count: # memory is full and thus page replacement is necessary.
				memory_pages[ fifo_index ] = page
				fifo_index = (fifo_index + 1) % memory_maximum_pages_count
				page_faults_count += 1
			else:
				raise_runtime_error_memory_pages_count_being_greater_than_memory_maximum_pages_count()
		else:
			# do nothing.
			pass
	return page_faults_count

def LRU(memory_maximum_pages_count, pages):
	memory_pages_count = 0
	memory_pages = []
	page_faults_count = 0
	for page in pages:
		if page not in memory_pages:
			if memory_pages_count < memory_maximum_pages_count: # memory is not full and thus no page replacement is necessary.
				memory_pages_count, page_faults_count = add_new_page_to_memory_pages(page, memory_pages, memory_pages_count, page_faults_count)
			elif memory_pages_count == memory_maximum_pages_count: # memory is full and thus page replacement is necessary.
				memory_pages.pop(0) # page at index 0 is the least-recently used (LRU) page.
				memory_pages.append(page)
				page_faults_count += 1
			else:
				raise_runtime_error_memory_pages_count_being_greater_than_memory_maximum_pages_count()
		else:
			# make the page become the most-recently used page.
			memory_pages.remove(page)
			memory_pages.append(page)
	return page_faults_count

def OPT(memory_maximum_pages_count, pages):
	memory_pages_count = 0
	memory_pages = []
	page_faults_count = 0
	current_page_in_pages_index = 0
	for page in pages:
		if page not in memory_pages:
			if memory_pages_count < memory_maximum_pages_count: # memory is not full and thus no page replacement is necessary.
				memory_pages_count, page_faults_count = add_new_page_to_memory_pages(page, memory_pages, memory_pages_count, page_faults_count)
			elif memory_pages_count == memory_maximum_pages_count: # memory is full and thus page replacement is necessary.
				furthest_to_be_used_memory_page_in_remaining_pages_index = -1
				to_be_evicted_page = None
				for memory_page in memory_pages:
					if memory_page not in pages[current_page_in_pages_index:]: # this page will never be used again.
						# immediately choose the current page to evict from the memory.
						to_be_evicted_page = memory_page
						break
					else:
						# find and evict the page that will not be used for the longest period of time.
						memory_page_in_remaining_pages_index = pages[current_page_in_pages_index:].index(memory_page)
						if memory_page_in_remaining_pages_index > furthest_to_be_used_memory_page_in_remaining_pages_index:
							furthest_to_be_used_memory_page_in_remaining_pages_index = memory_page_in_remaining_pages_index
							to_be_evicted_page = memory_page
				if to_be_evicted_page == None:
					raise RuntimeError('the optimal algorithm cannot find the page to evict from the memory')
				to_be_evicted_page_index = memory_pages.index(to_be_evicted_page)
				memory_pages.remove(to_be_evicted_page)
				memory_pages.insert(to_be_evicted_page_index, page)
				page_faults_count += 1
			else:
				raise_runtime_error_memory_pages_count_being_greater_than_memory_maximum_pages_count()
		else:
			# do nothing.
			pass
		current_page_in_pages_index += 1
	return page_faults_count

pages = (7, 2, 3, 1, 2, 5, 3, 4, 6, 7, 7, 1, 0, 5, 4, 6, 2, 3, 0, 1) # page reference string from problem 9.8.
memory_maximum_pages_count = int(sys.argv[1])
print('FIFO results in', FIFO(memory_maximum_pages_count, pages), 'page faults.')
print('LRU results in', LRU(memory_maximum_pages_count, pages), 'page faults.')
print('OPT results in', OPT(memory_maximum_pages_count, pages), 'page faults.')
