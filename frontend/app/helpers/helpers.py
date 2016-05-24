#! /usr/bin/env python
# coding: utf-8

import sys
import json
import urllib2
import httplib
import logging
import random
import socket
import timeit
import string
import re
import itertools

# general
RETRY_CNT = 3
KEY_RESPONSE = 'response'
API_URL = 'https://api.vk.com/method/{}?'

# get posts from group wall
PARAM_WALL_METHOD = 'wall.getById'
PARAM_POSTS = 'posts={}'
PARAM_COPY_HISTORY_DEPTH = 'copy_history_depth={}'

# get group titles
PARAM_GROUP_METHOD = 'groups.getById'
PARAM_GROUP_IDS = 'group_ids={}'


linebreak_killer = re.compile('( |<br>)+')
punctuation_marks = string.punctuation + u'«»'

def retry_func(func):
    def wrapper(*args, **kwargs):
        retries = RETRY_CNT
        while retries:
            out = func(*args, **kwargs)
            if out:
                return out
            logging.info('Retry function: %s' % func.__name__)
            retries -= 1
        logging.error('Retry function %s %d times with no succes' \
            % (func.__name__, RETRY_CNT))
        return None
    return wrapper


def time_exec(func):
    def wrapper(*args, **kwargs):
        s = timeit.default_timer()
        out = func(*args, **kwargs)
        f = timeit.default_timer()
        print '%s takes %f sec' % (func.__name__, f - s)
        sys.stdout.flush()
        return out
    return wrapper


class Result(object):
    def __init__(self, group_id = 0, post_id = 0, title = '', snippet = ''):
        self.group_id = group_id
        self.post_id = post_id
        self.title = title
        self.snippet = snippet


@time_exec
def post_query(server, port, query):
    if True:
        buf_size = 4096
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(30)
        
        s.connect((server, port))
        s.send("q={}".format(query))
        
        raw_reply = ''
        while True:
            reply = s.recv(buf_size)
            raw_reply += reply
            if len(reply) < buf_size:
                break
        s.close()

        reply = json.loads(raw_reply)
        if not reply or not reply.get("docs"):
            return {"terms": [], "docs": []}

        for doc in reply["docs"]:
            doc["group_id"] = int(doc["group_id"])
            doc["post_id"] = int(doc["post_id"])
            doc["rank"] = float(doc["rank"])
    
    else:
        fake_doc = lambda: { \
            "rank": random.uniform(0, 1), \
            "group_id": random.randint(0, 10**5), \
            "post_id": random.randint(1, 10**2) \
        }
        
        n = random.randint(0, 100)
        reply = {
            "terms": "the quick brown fox jumps over the lazy dog".split(),
            "docs": [
                fake_doc() for _ in xrange(n)
            ]
        }
    return reply


@time_exec
def make_results(docs, terms, snippet_distance, max_snippet_length):
    title_stub = "Название этой группы недоступно".decode("utf-8")
    text_stub = "Содержимое этого поста недоступно".decode("utf-8")
    
    gids = set(map(lambda d: d["group_id"], docs))
    gpids = set(map(lambda d: (d["group_id"], d["post_id"]), docs))
    titles, texts = get_group_titles(gids), get_post_texts(gpids)

    results = []
    for doc in docs:
        gid, pid = doc["group_id"], doc["post_id"]
        
        title, text = titles.get(gid, title_stub), texts.get((gid, pid))
        text = make_snippet(text, terms, snippet_distance, max_snippet_length) if text else text_stub

        r = Result(gid, pid, title, text)
        results.append(r)

    return results


def make_snippet(text, terms, dist, max_length):
    # eliminate <br> and get list of words
    single_line = linebreak_killer.sub(' ', text)
    split_text = single_line.split(' ')

    # split_text = single_line.encode('utf-8').translate(
    #     string.maketrans(string.punctuation, ' ' * len(string.punctuation))
    # ).decode('utf-8').split(' ')
    
    # tokenizer = lambda token: token.upper().strip(string.punctuation)
    
    # 'AB.C' -> ['AB', 'C']
    punct_split = lambda x: (''.join([ch.upper() if ch not in punctuation_marks else ' ' for ch in x])).split(' ')
    
    # whether a token or list of tokens starts with a normalized search term
    swc = lambda token, term: token.startswith(term) and sum(map(lambda l: l[0] == l[1], zip(token, term))) >= len(token) / 2.
    checker = lambda i, term: any([swc(t, term) for t in tokens[i]]) if isinstance(tokens[i], list) else swc(tokens[i], term)
    
    # find all words we need to highlight
    tokens = map(punct_split, split_text)
    shallow_occurences = [[i for i in xrange(len(tokens)) if checker(i, term)] for term in terms]
    occurences = sorted(list(itertools.chain(*shallow_occurences)))
    
    
    if not occurences:
        return text


    highlighted_text = map(lambda i: \
        '<b>%s</b>' % split_text[i] if i in occurences else split_text[i], \
        range(len(split_text))
    )
    result_tmp = []

    # build occurences list
    for i in xrange(len(occurences)):
        d = dist if i in [0, len(occurences)-1] else dist / 2
        result_tmp.append((max(0, occurences[i] - d), min(occurences[i] + d, len(highlighted_text))))
    
    # merge adjacent occurences, if necessary
    final_result = []
    for occ in result_tmp:
        if not final_result or occ[0] > final_result[-1][1]:
            final_result.append([occ[0], occ[1]])
        else:
            final_result[-1][1] = occ[1] 
    
    joiner = lambda l: ' '.join(l)
    separator = ' ... '
    
    # bang
    snippet_string = separator.join(map(joiner, [highlighted_text[occ[0]:occ[1]] for occ in final_result]))
    head = separator if final_result[0][0] > 0 else ''
    tail = separator if final_result[-1][1] < len(highlighted_text) else ''
    
    return head + snippet_string + tail

    # for i in xrange(len(t)):
    #     token = t[i].upper().strip(string.punctuation)
    #     for term in terms:
    #         if token.startswith(term):
    #             t[i] = '<b>%s</b>' % t[i]
    #             break
    #
    # return ' '.join(t)


def get_group_titles(gids):
    titles = {}
    if True:
        raw_reply = get_group_titles_from_api(gids)
        jr = json.loads(raw_reply)
        if KEY_RESPONSE in jr and jr[KEY_RESPONSE]:
            groups = jr[KEY_RESPONSE]
            for group in groups:
                titles[group["gid"]] = group["name"]
        return titles
    
    else:
        for g in gids:
            titles[g] = random.choice(["foo", "bar", "qoox", "zooz"])

    return titles


def get_post_texts(gpids):
    texts = {}
    if True:
        raw_reply = get_post_texts_from_api(gpids)
        jr = json.loads(raw_reply)
        if KEY_RESPONSE in jr and jr[KEY_RESPONSE]:
            posts = jr[KEY_RESPONSE]
            for post in posts:
                texts[(-post["to_id"], post["id"])] = post["text"]
        return texts
        
    else:
        for t in gpids:
            texts[t] = random.choice([
                "Lorem ipsum dolor sit amet.",
                "Deleniti et aut cumque aspernatur sit fugit reiciendis.",
                "Soluta sed magni quia quasi voluptatem saepe id.",
                "Repudiandae illum aut illo quis modi cupiditate praesentium."
            ])

    return texts


def get_group_titles_from_api(gids):
    gids_string = ','.join(map(str, gids))
    
    request_url = '&'.join([
        API_URL.format(PARAM_GROUP_METHOD),
        PARAM_GROUP_IDS
    ]).format(
        gids_string
    )

    return make_request(request_url)


def get_post_texts_from_api(gpids):
    gpids_string = ','.join(map(
        lambda x: '-{}_{}'.format(x[0], x[1]),
        gpids
    ))

    request_url = '&'.join([
        API_URL.format(PARAM_WALL_METHOD),
        PARAM_POSTS,
        PARAM_COPY_HISTORY_DEPTH
    ]).format(
        gpids_string,
        2
    )

    return make_request(request_url)


@retry_func
def make_request(request_url):    
    try:
        request = urllib2.urlopen(request_url)
        return request.read()
    except urllib2.HTTPError, e:
        logging.error('HTTPError = %s' % str(e.code))
    except urllib2.URLError, e:
        logging.error('URLError = %s' % str(e.reason))
    except httplib.HTTPException, e:
        logging.error('HTTPException: %s', str(e))
    except Exception, e:
        logging.error('Exception: %s', str(e))
    return None

