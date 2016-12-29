#!/usr/bin/env python
# -*- coding: utf-8 -*-

def send_twit(twit):
    import tweepy
    import secrets
    secrets = secrets.load()
    auth = tweepy.OAuthHandler(secrets['twitter_consumer_key'], secrets['twitter_consumer_secret'])
    auth.set_access_token(secrets['twitter_access_token'], secrets['twitter_access_token_secret'])

    api = tweepy.API(auth)
    return api.update_status(twit)
