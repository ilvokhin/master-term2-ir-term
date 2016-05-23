#! /usr/bin/env python
# -*- coding: utf-8 -*

import sys
import json
import logging
import urllib2
import httplib

OFFSET_SHIFT = 100
RETRY_CNT = 3
PARAM_OWNER_ID = 'owner_id=%d'
PARAM_OFFSET = 'offset=%d'
PARAM_COUNT = 'count=%d'
API_URL = 'https://api.vk.com/method/wall.get?'
KEY_RESPONSE = 'response'

def retry_func(func):
  def wrapper(*args, **kwargs):
    retries = RETRY_CNT
    while retries:
      out = func(*args, **kwargs)
      if out:
        return out
      logging.info('Retry function: %s' % func.__name__)
      retries -= 1
    logger.error('Retry function %s %d times with no succes' \
        % (func.__name__, RETRY_CNT))
    return None
  return wrapper

@retry_func
def make_request(owner_id, offset = -1, count = 100):
  param_owner = PARAM_OWNER_ID % owner_id
  param_offset = ''
  if offset != -1:
    param_offset = PARAM_OFFSET % offset
  param_count = PARAM_COUNT % count
  request_url = '&'.join([API_URL, param_owner, param_count, param_offset])
  logging.debug('Make request with url: %s' % request_url)
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
    logging.error('Expcetion: %s', str(e))
  return None

def get_last_posts(owner_id, offset):
  raw_response = make_request(owner_id, offset)
  if not raw_response:
    raise Exception('Can\'t get wall posts for user %d' % owner_id)
  parsed = json.loads(raw_response)
  if KEY_RESPONSE in parsed and len(parsed[KEY_RESPONSE]) != 1:
    return raw_response
  return None

def main():
  logging.basicConfig(level = logging.DEBUG)

  for line in sys.stdin:
    user_id = int(line.strip())
    offset = 0
    posts_data = get_last_posts(user_id, offset)
    while posts_data:
      print '\t'.join([str(user_id), posts_data])
      offset += OFFSET_SHIFT
      posts_data = get_last_posts(user_id, offset)

if __name__ == "__main__":
  main()
