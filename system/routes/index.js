var crypto = require('crypto');
var User = require('../models/user.js');
var Post = require('../models/post.js');
var fs = require('fs');
var hello = require('../addon/build/Release/hello.node')

module.exports = function(app) {
  app.get('/', function(req, res) {
    Post.get(null, function(err, posts) {
      if (err) {
        posts = [];
      }
      res.render('index', {
        title: '首頁',
        posts: posts,
      });
    });
  });
  
  app.get('/reg', function(req, res) {
    res.render('reg', {
      title: '检测',
    });
  });
  
  app.post('/reg', function(req, res) {
    console.log("a");
    
    var tmp_path = req.files.upload.path;
    var target_path = './files/' + req.files.upload.name;

    fs.rename(tmp_path, target_path, function(err){
        if (err) throw err;
        
        fs.unlink(tmp_path, function(){
            if (err) throw err;

            var result = hello.hello(target_path);
            //res.send('file uploaded ' + result);
            var ep = 0.4
            if (result > 0.4){
            res.render('dup', {
                title: 'dup',
                test: result + ' > ' + ep
            });
            }else{
            res.render('ndup', {
                title: 'ndup',
                test: result + ' < ' + ep
            });
            }
        });
    });
        
    //res.redirect('/');
  });
  
};

