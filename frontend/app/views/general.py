# coding:utf-8

from flask import Blueprint, render_template, redirect, url_for, request, flash, g

import socket
from app.helpers.helpers import (
    make_results,
    post_query
)

mod = Blueprint('general', __name__)

@mod.route('/')
def index():
    query, group_id = request.args.get('q', ''), int(request.args.get('group', -1))
    if query:
        query = query.encode("utf-8")
        server = "130.211.49.44" if False else "127.0.0.1"
        
        reply = post_query(server, 8888, query)
        if group_id != -1:
            reply["docs"] = filter(lambda x: x["group_id"] == group_id, reply["docs"])
        
        results = make_results(
            reply,
            snippet_distance=8,
            max_snippet_length=500
        )

                
        return render_template('results.html', query=query.decode("utf-8"), results=results)

    return render_template('index.html')

