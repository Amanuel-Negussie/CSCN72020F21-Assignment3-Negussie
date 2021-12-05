# a simple RESTful API

# to get collection:
# curl http://localhost:5000/posts
#
# to get one:
# curl http://localhost:5000/posts/4
#
# to delete one
# curl http://localhost:5000/posts/3  -X DELETE
#
# to add a new one:
# $ curl http://localhost:5000/posts -d "post=a new posting" -X POST
#
# to update an existing one:
# curl http://localhost:5000/posts/3 -d "post=something different" -X PUT
#
# run as regular user with command:
# python3 ./api.py
#
#
# adapted from https://flask-restful.readthedocs.io/en/latest/quickstart.html




from flask import Flask
from flask_restful import reqparse, abort, Api, Resource

app = Flask(__name__)
api = Api(app)

POSTS = {
    '1': {'post': 'First posting in posts'},
    '2': {'post': 'Another posting'},
    '3': {'post': 'Final starter post'},
}


def abort_if_post_doesnt_exist(post_id):
    if post_id not in POSTS:
        abort(404, message="Post {} doesn't exist".format(post_id))

parser = reqparse.RequestParser()
parser.add_argument('post')


class Post(Resource):
    def get(self, post_id):
        abort_if_post_doesnt_exist(post_id)
        return POSTS[post_id]

    def delete(self, post_id):
        abort_if_post_doesnt_exist(post_id)
        del POSTS[post_id]
        return '', 204

    def put(self, post_id):
        args = parser.parse_args()
        post = { 'post' : args['post']}
        POSTS[post_id] = post
        return post, 201


class PostList(Resource):
    def get(self):
        return POSTS   #we could parse the query string here and give only the requested posts

    def post(self):
        args = parser.parse_args()
        post_id = str(len(POSTS.keys()) + 1)
        POSTS[post_id] = {'post': args['post']}
        return POSTS[post_id], 201

##
## Actually setup the Api resource routing here
##
api.add_resource(PostList, '/posts')
api.add_resource(Post, '/posts/<post_id>')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
