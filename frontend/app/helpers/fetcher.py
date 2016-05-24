# coding: utf-8

from helpers import make_results

class Fetcher():
    def __init__(self, results_per_page = 10):
        self.start = 0
        self.results_per_page = results_per_page
        self.snippet_distance = 8
        self.max_snippet_length = 500

    def store(self, reply):
        self.start = 0
        self.docs = reply["docs"]
        try:
            self.docs.sort(key = lambda d: d["rank"], reverse = True)
        except AttributeError:
            print('AttributeError happened, docs: {}'.format(docs))

        self.terms = map(lambda t: t.encode('iso-8859-1').decode('utf-8'), reply["terms"])

    def next_page(self):
        page_end = min(self.start + self.results_per_page, len(self.docs))
        if self.start < len(self.docs):
            results = make_results(
                self.docs[self.start:page_end],
                self.terms,
                self.snippet_distance,
                self.max_snippet_length)

            self.start += len(results)
            return results
        else:
            return []
