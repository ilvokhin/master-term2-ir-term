# coding:utf-8

from flask import Blueprint, render_template, redirect, url_for, request, flash, g

import socket
from app.helpers.helpers import (
    make_results,
    post_query
)
from app.helpers.fetcher import Fetcher

mod = Blueprint('general', __name__)

results_per_page = 10
fetcher = Fetcher(results_per_page)

@mod.route('/')
def index():
    query, group_id = request.args.get('q', ''), int(request.args.get('group', -1))
    if query:
        query = query.encode("utf-8")
        server = "130.211.49.44" if False else "127.0.0.1"
        
        reply = post_query(server, 8888, query)
        if group_id != -1:
            reply["docs"] = filter(lambda x: x["group_id"] == group_id, reply["docs"])
        
        fetcher.store(reply)
        first_page_results = fetcher.next_page()
        first_page_rendered = render_template('results_block.html', results = first_page_results)
        return render_template(
            'results.html',
            query = query.decode("utf-8"),
            initial_contents = first_page_rendered,
            results_length = len(fetcher.docs),
            results_per_page = results_per_page
        )

    return render_template('index.html')

@mod.route('/next_page')
def next_page():
    return render_template('results_block.html', results = fetcher.next_page())
